#include <assert.h>  // assert
#include <float.h>  // DECIMAL_DIG
#include <math.h>  // pow, sin
#include <stddef.h>  // size_t, NULL
#include <stdio.h>  // fclose, fflush, fopen, fprintf, fscanf, printf, sprintf, sscanf, stderr, stdin, stdout, EOF, FILE
#include <stdlib.h>  // free, malloc, EXIT_FAILURE, EXIT_SUCCESS


#ifdef WITH_OMP
#include <omp.h>  // omp_get_*
#endif  // WITH_OMP


typedef double real_type;


struct Mesh
{
  real_type * points;

  size_t space_points;

  size_t time_points;
};


struct Mesh *
mesh_Allocate (void)
{
  const size_t mesh_bytes = sizeof (struct Mesh);
  const struct Mesh * const new_mesh = malloc (mesh_bytes);
  if (new_mesh == NULL)
  {
    fprintf (stderr, "Error: couldn't allocate memory for mesh (%zu bytes).\n", mesh_bytes);

    return NULL;
  }

  return new_mesh;
}


struct Mesh *
mesh_Construct (size_t time_points, size_t space_points)
{
  assert (time_points > 1);
  assert (space_points > 0);

  struct Mesh * const new_mesh = mesh_Allocate ();
  if (new_mesh == NULL)
  {
    return NULL;
  }

  const size_t points_bytes = time_points * space_points * sizeof (real_type);
  new_mesh->points = malloc (points_bytes);
  if (new_mesh->points == NULL)
  {
    fprintf (stderr, "Error: couldn't allocate memory for mesh points (%zu bytes).\n", points_bytes);

    free (new_mesh);

    return NULL;
  }

  new_mesh->space_points = space_points;
  new_mesh->time_points = time_points;

  return new_mesh;
}


void
mesh_Destroy (struct Mesh * mesh)
{
  if (mesh != NULL)
  {
    free (mesh->points);
  }

  free (mesh);
}


inline size_t
mesh_PointIndex (const struct Mesh * mesh, size_t time_point, size_t space_point)
{
  assert (mesh != NULL);

  return time_point % mesh->time_points * mesh->space_points + space_point;
}


inline real_type
mesh_Get (const struct Mesh * mesh, size_t time_point, size_t space_point)
{
  assert (mesh != NULL);

  return mesh->points [mesh_PointIndex (mesh, time_point, space_point)];
}


inline void
mesh_Set (const struct Mesh * mesh, size_t time_point, size_t space_point, real_type new_value)
{
  assert (mesh != NULL);

  mesh->points [mesh_PointIndex (mesh, time_point, space_point)] = new_value;
}


int
mesh_Write_File (const struct Mesh * mesh, FILE * output, const char * restrict format)
{
  assert (mesh != NULL);
  assert (output != NULL);
  assert (format != NULL);

  fprintf (output, "Mesh{points={");
  for (size_t time_point = 0; time_point < mesh->time_points; ++ time_point)
  {
    for (size_t space_point = 0; space_point < mesh->space_points; ++ space_point)
    {
      fprintf (output, format, mesh_Get (mesh, time_point, space_point));
      fprintf (output, ";");
    }
  }
  fprintf (output, "};space_points=%zu;time_points=%zu;}", mesh->space_points, mesh->time_points);

  return 0;
}


int
mesh_Write_File_Formatted (const struct Mesh * mesh, FILE * output, const char * restrict format)
{
  assert (mesh != NULL);
  assert (output != NULL);
  assert (format != NULL);

  for (size_t time_point = 0; time_point < mesh->time_points; ++ time_point)
  {
    for (size_t space_point = 0; space_point < mesh->space_points; ++ space_point)
    {
      fprintf (output, format, mesh_Get (mesh, time_point, space_point));
    }

    if (time_point != mesh->time_points - 1)
    {
      fprintf (output, "\n");
    }
  }

  return 0;
}


int
mesh_Write_File_Formatted_AtTimePoint (
  const struct Mesh * mesh, FILE * output, const char * restrict format, size_t time_point
)
{
  assert (mesh != NULL);
  assert (output != NULL);
  assert (format != NULL);

  for (size_t space_point = 0; space_point < mesh->space_points; ++ space_point)
  {
    fprintf (output, format, mesh_Get (mesh, time_point, space_point));
  }

  return 0;
}


typedef real_type temperature_function_type (real_type);


/**
 * @brief ∂u/∂t = α∂²u/∂x²
 * u(x, 0) = f(x)
 * u(0, t) = β₀
 * u(L, t) = β₁
 * t ∈ [0, T]
 * x ∈ [0, L]
 * TODO:  Store initial condition values directly.
 * TODO:  Infer β₀  &  β₁ from initial condition values.
 */
struct Parameters
{
  /**
   * @brief β₀
   */
  real_type boundary_condition_0;

  /**
   * @brief β₁
   */
  real_type boundary_condition_1;

  /**
   * @brief α
   */
  real_type diffusivity;

  /**
   * @brief f(x)
   */
  temperature_function_type * initial_condition;

  /**
   * @brief L
   */
  real_type space_max;

  size_t space_points;

  /**
   * @brief T
   */
  real_type time_max;

  size_t time_points;
};


struct Parameters *
parameters_Allocate (void)
{
  const size_t bytes = sizeof (struct Parameters);
  const struct Parameters * const new_parameters = malloc (bytes);
  if (new_parameters == NULL)
  {
    fprintf (stderr, "Error: couldn't allocate memory for parameters (%zu bytes).\n", bytes);

    return NULL;
  }

  return new_parameters;
}


struct Parameters *
parameters_Construct (
  real_type diffusivity, temperature_function_type * initial_condition,
  real_type boundary_condition_0, real_type boundary_condition_1,
  real_type time_max, real_type space_max,
  size_t time_points, size_t space_points
)
{
  assert (initial_condition != NULL);
  assert (time_points > 1);
  assert (space_points > 1);

  struct Parameters * const new_parameters = parameters_Allocate ();
  if (new_parameters == NULL)
  {
    return NULL;
  }

  new_parameters->boundary_condition_0 = boundary_condition_0;
  new_parameters->boundary_condition_1 = boundary_condition_1;
  new_parameters->diffusivity = diffusivity;
  new_parameters->initial_condition = initial_condition;
  new_parameters->space_max = space_max;
  new_parameters->space_points = space_points;
  new_parameters->time_max = time_max;
  new_parameters->time_points = time_points;

  return new_parameters;
}


void
parameters_Destroy (struct Parameters * parameters)
{
  free (parameters);
}


int
parameters_Write_File (const struct Parameters * parameters, FILE * output)
{
  assert (parameters != NULL);
  assert (output != NULL);

  return fprintf (
    output,
    "Parameters{boundary_condition_0=%.*f;boundary_condition_1=%.*f;diffusivity=%.*f;space_max=%.*f;space_points=%zu;time_max=%.*f;time_points=%zu;}",
    DECIMAL_DIG, parameters->boundary_condition_0, DECIMAL_DIG, parameters->boundary_condition_1,
    DECIMAL_DIG, parameters->diffusivity, DECIMAL_DIG, parameters->space_max, parameters->space_points,
    DECIMAL_DIG, parameters->time_max, parameters->time_points
  );
}


#define PARAMETERS_STRING_BUFFER_LENGTH (384)
#define PARAMETERS_MEMBERS_COUNT (7)


struct Parameters *
parameters_Read_File (FILE * input)
{
  assert (input != NULL);

  struct Parameters * const new_parameters = parameters_Allocate ();
  if (new_parameters == NULL)
  {
    fprintf (stderr, "Error: couldn't allocate parameters.\n");

    return NULL;
  }

  static char buffer [PARAMETERS_STRING_BUFFER_LENGTH];
  size_t buffer_length = 0;
  const int assigned_buffer = fscanf (input, "%[^\n]%zn", buffer, & buffer_length);
  if (assigned_buffer != 1) {
    if (assigned_buffer == EOF) {
      fprintf (stderr, "Error: unexpected end of input.\n");
    } else {
      fprintf (stderr, "Error: blank input.\n");
    }

    parameters_Destroy (new_parameters);

    return NULL;
  }

  const int assigned_parameters = sscanf (
    buffer,
    "Parameters{boundary_condition_0=%lf;boundary_condition_1=%lf;diffusivity=%lf;space_max=%lf;space_points=%zu;time_max=%lf;time_points=%zu;};",
    & new_parameters->boundary_condition_0, & new_parameters->boundary_condition_1, & new_parameters->diffusivity,
    & new_parameters->space_max, & new_parameters->space_points, & new_parameters->time_max,
    & new_parameters->time_points
  );
  if (assigned_parameters != PARAMETERS_MEMBERS_COUNT)
  {
    fprintf (stdout, "Error: couldn't assign parameters.\n");

    parameters_Destroy (new_parameters);

    return NULL;
  }

  return new_parameters;
}


inline real_type
lerp (real_type x, real_type x_0, real_type x_1, real_type y_0, real_type y_1)
{
  return y_0 + (x - x_0) * (y_1 - y_0) / (x_1 - x_0);
}


typedef int solution_visitor_type (const struct Parameters * parameters, const struct Mesh * mesh, size_t time_point);


#define METHOD_EULER (1)
#define METHOD_RK4 (METHOD_EULER + 1)


inline real_type
f (real_type space_step, real_type time_step, real_type diffusivity, real_type u_0, real_type u_1, real_type u_2)
{
  return time_step
    * diffusivity
    * (
                u_0
        - 2.0 * u_1
        +       u_2
      )
    / pow (space_step, 2.0);
}


int
solve (
  const struct Parameters * parameters, solution_visitor_type * before_solution, solution_visitor_type * on_solution
)
{
  assert (parameters != NULL);

  if (before_solution != NULL)
  {
    const int visited = before_solution (parameters, NULL, - 1);
    if (visited != 0)
    {
      fprintf (stderr, "Error: something went wrong.\n");

      return visited;
    }
  }

  const struct Mesh * const mesh = mesh_Construct (2, parameters->space_points);
  if (mesh == NULL)
  {
    fprintf (stderr, "Error: couldn't allocate memory for mesh.\n");

    return - 1;
  }

  mesh_Set (mesh, 0, 0, parameters->boundary_condition_0);
  mesh_Set (mesh, 0, mesh->space_points - 1, parameters->boundary_condition_1);
  for (size_t space_point = 1; space_point < mesh->space_points - 1; ++ space_point)
  {
    const real_type space = lerp (
      (real_type) space_point, 0.0, (real_type) (mesh->space_points - 1), 0.0, parameters->space_max
    );
    const real_type temperature = parameters->initial_condition (space);
    mesh_Set (mesh, 0, space_point, temperature);
  }

  if (on_solution != NULL)
  {
    const int visited = on_solution (parameters, mesh, 0);
    if (visited != 0)
    {
      fprintf (stderr, "Error: something went wrong.\n");

      mesh_Destroy (mesh);

      return visited;
    }
  }

  const real_type time_step = parameters->time_max / (real_type) parameters->time_points;
  const real_type space_step = parameters->space_max / (real_type) parameters->space_points;
#if METHOD == METHOD_EULER
  const real_type r = parameters->diffusivity * (time_step / pow (space_step, 2.0));
  assert (r <= 0.5);
#endif  // METHOD == METHOD_EULER
  for (size_t time_point = 1; time_point < parameters->time_points; ++ time_point)
  {
    mesh_Set (mesh, time_point, 0, parameters->boundary_condition_0);
    mesh_Set (mesh, time_point, mesh->space_points - 1, parameters->boundary_condition_1);
#ifdef WITH_OMP
#pragma omp parallel for
#endif  // WITH_OMP
    for (size_t space_point = 1; space_point < mesh->space_points - 1; ++ space_point)
    {
#if METHOD == METHOD_EULER
      /*
       * Forward-time central-space scheme.
       * Time:  1st order forward difference  (explicit Euler method aka explicit 1-st order Runge-Kutta method).
       * Space:  2nd order central difference.
       */
/*      const real_type temperature =
          mesh_Get (mesh, time_point - 1, space_point)
        + time_step
        * parameters->diffusivity
        * (
                    mesh_Get (mesh, time_point - 1, space_point - 1)
            - 2.0 * mesh_Get (mesh, time_point - 1, space_point    )
            +       mesh_Get (mesh, time_point - 1, space_point + 1)
          )
        / pow (space_step, 2.0);*/
      const real_type temperature =
          (1.0 - 2.0 * r) * mesh_Get (mesh, time_point - 1, space_point    )
        +              r  * mesh_Get (mesh, time_point - 1, space_point - 1)
        +              r  * mesh_Get (mesh, time_point - 1, space_point + 1);
      mesh_Set (mesh, time_point, space_point, temperature);
#elif METHOD == METHOD_RK4
      /**
       * Time:  explicit 4-th order Runge-Kutta method.
       * Space:  2nd order central difference.
       */
/*      const real_type k_1 =
          time_step
        * parameters->diffusivity
        * (
                    mesh_Get (mesh, time_point - 1, space_point - 1)
            - 2.0 * mesh_Get (mesh, time_point - 1, space_point    )
            +       mesh_Get (mesh, time_point - 1, space_point + 1)
          )
        / pow (space_step, 2.0);
      const real_type k_2 =
          time_step
        * parameters->diffusivity
        * (
                    (mesh_Get (mesh, time_point - 1, space_point - 1) + k_1 / 2.0)
            - 2.0 * (mesh_Get (mesh, time_point - 1, space_point    ) + k_1 / 2.0)
            +       (mesh_Get (mesh, time_point - 1, space_point + 1) + k_1 / 2.0)
          )
        / pow (space_step, 2.0);
      const real_type k_3 =
          time_step
        * parameters->diffusivity
        * (
                    (mesh_Get (mesh, time_point - 1, space_point - 1) + k_2 / 2.0)
            - 2.0 * (mesh_Get (mesh, time_point - 1, space_point    ) + k_2 / 2.0)
            +       (mesh_Get (mesh, time_point - 1, space_point + 1) + k_2 / 2.0)
          )
        / pow (space_step, 2.0);
      const real_type k_4 =
          time_step
        * parameters->diffusivity
        * (
                    (mesh_Get (mesh, time_point - 1, space_point - 1) + k_3)
            - 2.0 * (mesh_Get (mesh, time_point - 1, space_point    ) + k_3)
            +       (mesh_Get (mesh, time_point - 1, space_point + 1) + k_3)
          )
        / pow (space_step, 2.0);*/
      const real_type k_1 = f (
        space_step, time_step, parameters->diffusivity,
        mesh_Get (mesh, time_point - 1, space_point - 1),
        mesh_Get (mesh, time_point - 1, space_point    ),
        mesh_Get (mesh, time_point - 1, space_point + 1)
      );
      const real_type k_2 = f (
        space_step, time_step, parameters->diffusivity,
        mesh_Get (mesh, time_point - 1, space_point - 1) + k_1 / 2.0,
        mesh_Get (mesh, time_point - 1, space_point    ) + k_1 / 2.0,
        mesh_Get (mesh, time_point - 1, space_point + 1) + k_1 / 2.0
      );
      const real_type k_3 = f (
        space_step, time_step, parameters->diffusivity,
        mesh_Get (mesh, time_point - 1, space_point - 1) + k_2 / 2.0,
        mesh_Get (mesh, time_point - 1, space_point    ) + k_2 / 2.0,
        mesh_Get (mesh, time_point - 1, space_point + 1) + k_2 / 2.0
      );
      const real_type k_4 = f (
        space_step, time_step, parameters->diffusivity,
        mesh_Get (mesh, time_point - 1, space_point - 1) + k_3,
        mesh_Get (mesh, time_point - 1, space_point    ) + k_3,
        mesh_Get (mesh, time_point - 1, space_point + 1) + k_3
      );
      const real_type temperature =
          mesh_Get (mesh, time_point - 1, space_point)
        + (
                    k_1
            + 2.0 * k_2
            + 2.0 * k_3
            +       k_4
          )
        / 6.0;
      mesh_Set (mesh, time_point, space_point, temperature);
#else  // METHOD == METHOD_RK4
#error "Unsupported method."
#endif  // METHOD == METHOD_EULER
    }

    if (on_solution != NULL)
    {
      const int visited = on_solution (parameters, mesh, time_point);
      if (visited != 0)
      {
        fprintf (stderr, "Error: something went wrong.\n");

        mesh_Destroy (mesh);

        return visited;
      }
    }
  }

  return 0;
}


int
writeSolution_Stdout (const struct Parameters * parameters_, const struct Mesh * mesh, size_t time_point)
{
  fprintf (stdout, "time_point=%zu;\n", time_point);
  mesh_Write_File (mesh, stdout, "%f");
//  mesh_Write_File_Formatted (mesh, stdout, "\t% f");
//  mesh_Write_File_Formatted_AtTimePoint (mesh, stdout, "\t% f", time_point);
  fprintf (stdout, ";\n");

  return 0;
}


#define PLOT_EVERY_NTH_SOLUTION (10)
#define GNUPLOT_SCRIPT_NAME ("plot.gp")
#define GNUPLOT_SCRIPT_TEMPLATE ( \
  "set terminal wxt persist\n" \
  "set xlabel 'Space (x)'\n" \
  "set ylabel 'Temperature (u(x))'\n" \
  "#set offsets graph 0.01, 0.01, 0.01, 0.01\n" \
  "set grid xtics ytics\n" \
  "set style fill transparent solid 0.333\n" \
  "filename(x) = sprintf(\"%%d.dat\", x)\n" \
  "set key autotitle columnhead\n" \
  "plot for [i = 0:%zu:%zu] filename(i) using 1:2 with lines\n" \
)


int
writeGnuplotScript_File (const struct Parameters * parameters, const struct Mesh * mesh_, size_t time_point_)
{
  FILE * const output = fopen (GNUPLOT_SCRIPT_NAME, "w");
  if (output == NULL)
  {
    fprintf (stderr, "Error: couldn't open output file (%s).\n", GNUPLOT_SCRIPT_NAME);

    return - 1;
  }

  fprintf (output, GNUPLOT_SCRIPT_TEMPLATE, parameters->time_points - 1, PLOT_EVERY_NTH_SOLUTION);

  const int flushed = fflush (output);
  if (flushed != 0)
  {
    fprintf (stderr, "Error: couldn't flush output file (%s).\n", GNUPLOT_SCRIPT_NAME);

    return flushed;
  }

  const int closed = fclose (output);
  if (closed != 0)
  {
    fprintf (stderr, "Error: couldn't close output file (%s).\n", GNUPLOT_SCRIPT_NAME);

    return closed;
  }

  return 0;
}


#define SOLUTION_FILENAME_BUFFER_LENGTH (48)
#define WRITE_EVERY_NTH_SOLUTION (10)


int
writeSolution_File (const struct Parameters * parameters, const struct Mesh * mesh, size_t time_point)
{
  if (time_point % WRITE_EVERY_NTH_SOLUTION != 0)
  {
    return 0;
  }

  static char filename [SOLUTION_FILENAME_BUFFER_LENGTH];
  sprintf (filename, "%zu.dat", time_point);
  FILE * const output = fopen (filename, "w");
  if (output == NULL)
  {
    fprintf (stderr, "Error: couldn't open output file (%s).\n", filename);

    return - 1;
  }

  const real_type time = lerp (
    (real_type) time_point, 0.0, (real_type) (parameters->time_points - 1), 0.0, parameters->time_max
  );
  fprintf (output, "x \"u(x, %g)\"\n", time);
  fprintf (output, "# ");
  parameters_Write_File (parameters, output);
  fprintf (output, ";\n");
  fprintf (output, "# time=%f;time_point=%zu;\n", time, time_point);
  fprintf (output, "# space;temperature;\n");

  for (size_t space_point = 0; space_point < parameters->space_points; ++ space_point)
  {
    const real_type space = lerp (
      (real_type) space_point, 0.0, (real_type) (mesh->space_points - 1), 0.0, parameters->space_max
    );
    const real_type temperature = mesh_Get (mesh, time_point, space_point);
    fprintf (output, "%.*f %.*f\n", DECIMAL_DIG, space, DECIMAL_DIG, temperature);
  }

  const int flushed = fflush (output);
  if (flushed != 0)
  {
    fprintf (stderr, "Error: couldn't flush output file (%s).\n", filename);

    return flushed;
  }

  const int closed = fclose (output);
  if (closed != 0)
  {
    fprintf (stderr, "Error: couldn't close output file (%s).\n", filename);

    return closed;
  }

  return 0;
}


#define PI (3.141592653589793238462643383279502884)


real_type
initialCondition (real_type space)
{
  return sin (space + 0.5 * PI);
}


#define INPUT_DEFAULT (1)
#define INPUT_STDIN (INPUT_DEFAULT + 1)


int
run (void)
{
#if INPUT == INPUT_DEFAULT
  struct Parameters * const parameters = parameters_Construct (1.0, initialCondition, 1.0, - 1.0, 5.0, 5.0 * PI, 1000 + 1, 100);
  if (parameters == NULL)
  {
    fprintf (stderr, "Error: couldn't allocate memory for parameters, exiting.\n");

    return EXIT_FAILURE;
  }
#elif INPUT == INPUT_STDIN
  struct Parameters * const parameters = parameters_Read_File (stdin);
  if (parameters == NULL)
  {
    fprintf (stderr, "Error: couldn't read parameters, exiting.\n");

    return EXIT_FAILURE;
  }

  parameters_Write_File (parameters, stdout);
  fprintf (stdout, ";\n");

  parameters->initial_condition = initialCondition;
#else  // INPUT == INPUT_STDIN
#error "Unsupported input."
#endif  // INPUT == INPUT_DEFAULT

#ifdef WITH_OMP
  printf (
    "max_threads=%d;num_threads=%d;num_procs=%d;thread_num=%d;\n",
    omp_get_max_threads (), omp_get_num_threads (), omp_get_num_procs (), omp_get_thread_num ()
  );
#endif //  WITH_OMP

  solution_visitor_type * const before_solution = writeGnuplotScript_File;
//  solution_visitor_type * const on_solution = writeSolution_Stdout;
  solution_visitor_type * const on_solution = writeSolution_File;
  const int solved = solve (parameters, before_solution, on_solution);
  if (solved != 0)
  {
    fprintf (stderr, "Error: couldn't solve, exiting.\n");

    parameters_Destroy (parameters);

    return EXIT_FAILURE;
  }

  parameters_Destroy (parameters);

  return EXIT_SUCCESS;
}


int
main (int argc, char * argv [])
{
  const int done = run ();

  printf ("Done.\n");

  return done;
}
