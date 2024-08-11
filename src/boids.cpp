#define MAX_BOIDS 2000

typedef struct boid_t Boid;
struct boid_t {
	vec3 pos;
	vec3 vel;
};


typedef struct param_t Param;
struct param_t {
	F32 r;
	F32 theta_max;

	F32 c;

	F32 s_r;
	F32 s;

	F32 a;

	F32 max_vel;
	F32 size;

	I32  simulation_space[3];
};


typedef struct boids_application_t BoidsApplication;
struct boids_application_t {
	B32   paused;
	Param p;
	I32   n;
	Boid  *bs;
};

internal B32
has_influence(Boid *b_j, Boid *b_i, Param *p) {
	vec3 v_rel;
	vec3_sub(v_rel, b_j->pos, b_i->pos);
	if (vec3_len(v_rel) > p->r) {
		return false;
	}
	F32 norm = Abs(vec3_len(b_i->vel)) * Abs(vec3_len(v_rel));
	F32 theta = acos(vec3_mul_inner(b_i->vel, v_rel) / norm);
	return theta < p->theta_max;
}

internal void
get_influencers(Boid *b, Boid *bs, I32 n, Param *p,
		Boid *influencers[], I32 *m) {
	I32 k = 0;
	for (I32 i = 0; i < n; ++i) {
		if (b != &bs[i] && has_influence(b, &bs[i], p)) {
			influencers[k] = &bs[i];
			++k;
		}
	}
	*m = k;
}

internal inline void
cohesion(vec3 r, Boid *b, Boid *influencers[], I32 m, Param *p) {
	if (m == 0) {
		return;
	}
	memset(r, 0, sizeof(vec3));
	for (I32 i = 0; i < m; ++i) {
		vec3_add(r, r, influencers[i]->pos);
	}
	vec3_scale(r, r, 1.0f/(F32)m);
	vec3_sub(r, r, b->pos);
	vec3_scale(r, r, p->c);
}

internal inline void
seperation(vec3 r, Boid *b, Boid *influencers[], I32 m, Param *p) {
	memset(r, 0, sizeof(vec3));
	vec3 dr;
	for (I32 i = 0; i < m; ++i) {
		vec3_sub(dr, influencers[i]->pos, b->pos);
		if (vec3_len(dr) < p->s_r) {
			vec3_sub(r, r, dr);
		}
	}
	vec3_scale(r, r, p->s);
}

internal inline void
alignment(vec3 r, Boid *b, Boid *influencers[], I32 m, Param *p) {
	if (m == 0) {
		return;
	}
	memset(r, 0, sizeof(vec3));
	for (I32 i = 0; i < m; ++i) {
		vec3_add(r, r, influencers[i]->vel);
	}
	vec3_scale(r, r, 1.0f/(F32)m);
	vec3_sub(r, r, b->vel);
	vec3_scale(r, r, p->a);
}

internal void
update_boid(BoidsApplication *app, Boid *b) {
	// BUG(Elias): Boids are going out of bounds on the right and top
	// borders.
	Boid *bs = app->bs;
	Param *p = &app->p;
	I32    n = app->n;

	Boid *influencers[n];
	I32 m;
	get_influencers(b, bs, n, p, influencers, &m);

	vec3 coh, sep, ali;
	cohesion(coh, b, influencers, m, p);
	seperation(sep, b, influencers, m, p);
	alignment(ali, b, influencers, m, p);
	vec3_add(b->vel, b->vel, coh);
	vec3_add(b->vel, b->vel, sep);
	vec3_add(b->vel, b->vel, ali);

	b->vel[0] = Clamp(-p->max_vel, b->vel[0], p->max_vel);
	b->vel[1] = Clamp(-p->max_vel, b->vel[1], p->max_vel);
	b->vel[2] = Clamp(-p->max_vel, b->vel[2], p->max_vel);

	vec3_add(b->pos, b->pos, b->vel);

	if (b->pos[0] <= 0) {
		b->vel[0] = Abs(b->vel[0]);
	} else if (b->pos[0] > app->p.simulation_space[0]) {
		b->vel[0] = -Abs(b->vel[0]);
	}
	if (b->pos[1] <= 0) {
		b->vel[1] = Abs(b->vel[1]);
	} else if (b->pos[1] > app->p.simulation_space[1]) {
		b->vel[1] = -Abs(b->vel[1]);
	}
	if (b->pos[2] <= 0) {
		b->vel[2] = Abs(b->vel[2]);
	} else if (b->pos[2] > app->p.simulation_space[2]) {
		b->vel[2] = -Abs(b->vel[2]);
	}
}

internal void
update_boids(BoidsApplication *app) {
	if (app->paused) {
		return;
	}
	for (I32 i = 0; i < app->n; ++i) {
		update_boid(app, &(app->bs[i]));
	}
}

internal void
init_boid(Boid *b, Param *p) {
	b->pos[0] = rand() % p->simulation_space[0];
	b->pos[1] = rand() % p->simulation_space[1];
	b->pos[2] = rand() % p->simulation_space[2];
	b->vel[0] = (rand() % 100 - 50) / 50.0f;
	b->vel[1] = (rand() % 100 - 50) / 50.0f;
}

internal void
init_boids_app(BoidsApplication *app) {
	Param *p = &app->p;
	app->n = 2000;
	p->r = 50;
	p->theta_max = 3.14 / 5;
	p->c = 0.01;
	p->s_r = 10;
	p->s = 0.1;
	p->a = 0.1;
	p->max_vel = 10.0;
	p->size = 1.0f;
	p->simulation_space[0] = 500;
	p->simulation_space[1] = 500;
	p->simulation_space[2] = 500;

	app->bs = (Boid *)calloc(MAX_BOIDS, sizeof(Boid));
	for (I32 i = 0; i < MAX_BOIDS; ++i) {
		init_boid(&app->bs[i], p);
	}
}

internal void
boids_app_die(BoidsApplication *app) {
	free(app->bs);
}



