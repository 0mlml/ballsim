#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#define PI 3.142857
#define WIDTH 1366
#define HEIGHT 768
#define FRAMERATE 50

#define NUM_BALLS 30
#define COEFF_RESTITUTION 0.3
#define GRAVITY 0.2
#define FRICTION 0.02

struct ball
{
	double r;
	double m;

	double p_x;
	double p_y;

	double v_x;
	double v_y;

	double a_x;
	double a_y;
};

struct ball balls[NUM_BALLS];

void initBalls()
{
	srand(time(NULL));
	for (int i = 0; i < NUM_BALLS; i++)
	{
		balls[i].p_x = 30 * i + 100;
		balls[i].p_y = 50;

		balls[i].v_x = 0;
		balls[i].v_y = 0;

		balls[i].a_x = 0;
		balls[i].a_y = 0;

		balls[i].m = rand() % 10;
		balls[i].r = balls[i].m * 8;
	}
}

void updateBalls()
{
	for (int i = 0; i < NUM_BALLS; i++)
	{
		for (int j = 0; j < NUM_BALLS; j++)
		{
			if (j == i)
				continue;
			// this fucking hurts
			double x_pos_diff = balls[i].p_x - balls[j].p_x,
				   y_pos_diff = balls[i].p_y - balls[j].p_y,
				   r_sum = balls[i].r + balls[j].r;
			if (x_pos_diff * x_pos_diff + y_pos_diff * y_pos_diff <= r_sum * r_sum) // x diff sq + y diff sq < r diff sq
			{
				double delta_x = balls[i].p_x - balls[j].p_x,
					   delta_y = balls[i].p_y - balls[j].p_y,
					   dist = sqrt(delta_x * delta_x + delta_y * delta_y),
					   mindist_x = delta_x * (((balls[i].r + balls[j].r) - dist) / dist),
					   mindist_y = delta_y * (((balls[i].r + balls[j].r) - dist) / dist),
					   impulse1 = 1 / balls[i].m,
					   impulse2 = 1 / balls[j].m;

				balls[i].p_x += mindist_x * (impulse1 / (impulse1 + impulse2));
				balls[i].p_y += mindist_y * (impulse1 / (impulse1 + impulse2));

				balls[j].p_x -= mindist_x * (impulse2 / (impulse1 + impulse2));
				balls[j].p_y -= mindist_y * (impulse2 / (impulse1 + impulse2));

				double v_x = balls[i].v_x - balls[j].v_x,
					   v_y = balls[i].v_y - balls[j].v_y,
					   mindist_mag = sqrt(mindist_x * mindist_x + mindist_y * mindist_y),
					   mindist_n_x = mindist_x / mindist_mag,
					   mindist_n_y = mindist_y / mindist_mag,
					   vn = v_x * mindist_n_x + v_y * mindist_n_y;

				// if (vn > 0)
				// 	return;

				double imp = (-(1 + COEFF_RESTITUTION) * vn) / (impulse1 + impulse2),
					   impulse_x = mindist_n_x * imp,
					   impulse_y = mindist_n_y * imp;

				balls[i].v_x += impulse_x * impulse1;
				balls[i].v_y += impulse_y * impulse1;

				balls[j].v_x -= impulse_x * impulse2;
				balls[j].v_y -= impulse_y * impulse2;

				printf("\rCollided ball[%d] (m: %lf) with ball[%d] (m: %lf) impl_x: %lf; impl_y: %lf", i, balls[i].m, j, balls[j].m, impulse_x, impulse_y);
			}
		}

		balls[i].a_x += -1 * balls[i].v_x * GRAVITY * balls[i].m * FRICTION;
		balls[i].v_x += balls[i].a_x;

		balls[i].a_y += GRAVITY;
		balls[i].v_y += balls[i].a_y;

		balls[i].a_x = 0;
		balls[i].a_y = 0;

		if (balls[i].p_x < balls[i].r)
		{
			balls[i].p_x = balls[i].r;
			balls[i].v_x *= -1 * COEFF_RESTITUTION;
		}
		else if (balls[i].p_x > WIDTH - balls[i].r)
		{
			balls[i].p_x = WIDTH - balls[i].r;
			balls[i].v_x *= -1 * COEFF_RESTITUTION;
		}

		if (balls[i].p_y < balls[i].r)
		{
			balls[i].p_y = balls[i].r + 1;
			balls[i].v_y *= -1 * COEFF_RESTITUTION;
		}
		else if (balls[i].p_y > HEIGHT - balls[i].r - 1)
		{
			balls[i].p_y = HEIGHT - balls[i].r;
			balls[i].v_y *= -1 * COEFF_RESTITUTION;
		}

		balls[i].p_x += balls[i].v_x;
		balls[i].p_y += balls[i].v_y;
	}
}

void doInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glColor3f(0.0, 1.0, 0.0);

	glPointSize(1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0, WIDTH, HEIGHT, 0);

	initBalls();
}

void drawCircle(int xoff, int yoff, int r)
{
	glBegin(GL_POINTS);
	float x, y, i;

	for (i = 0; i < (2 * PI); i += 0.001)
	{
		x = r * cos(i) + xoff;
		y = r * sin(i) + yoff;

		glVertex2i(x, y);
	}

	glEnd();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	updateBalls();
	for (int i = 0; i < NUM_BALLS; i++)
	{
		drawCircle(balls[i].p_x, balls[i].p_y, balls[i].r);
	}

	glFlush();

	glFinish();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(200, 200);

	glutCreateWindow("Ball simulation");
	doInit();

	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutMainLoop(); // Blocking loop
}
