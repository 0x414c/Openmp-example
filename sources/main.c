#include <assert.h>  // assert
#include <math.h>  // pow, sin
#include <stddef.h>  // size_t, NULL
#include <stdio.h>  // fclose, fflush, fopen, fprintf, sprintf, stderr, stdout, FILE
#include <stdlib.h>  // malloc, free, EXIT_FAILURE, EXIT_SUCCESS


typedef double real_type;


struct Mesh
{
  real_type * points;

  size_t space_points;

  size_t time_points;
};


struct Mesh *
mesh_Construct (size_t time_points, size_t space_points)
{
  assert (time_points > 1);
  assert (space_points > 0);

  const size_t mesh_bytes = sizeof (struct Mesh);
  struct Mesh * const new_mesh = malloc (mesh_bytes);
  if (new_mesh == NULL)
  {
    fprintf (stderr, "Error: couldn't allocate memory for mesh (%zu bytes).\n", mesh_bytes);

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
  assert (mesh != NULL);

  free (mesh->points);
  free (mesh);
}


size_t
mesh_PointIndex (const struct Mesh * mesh, size_t time_point, size_t space_point)
{
  assert (mesh != NULL);

  return ((time_point % mesh->time_points) * mesh->space_points) + space_point;
}


real_type
mesh_Get (const struct Mesh * mesh, size_t time_point, size_t space_point)
{
  assert (mesh != NULL);

  return mesh->points [mesh_PointIndex (mesh, time_point, space_point)];
}


void
mesh_Set (const struct Mesh * mesh, size_t time_point, size_t space_point, real_type new_value)
{
  assert (mesh != NULL);

  mesh->points [mesh_PointIndex (mesh, time_point, space_point)] = new_value;
}


void
mesh_Print (const struct Mesh * mesh, FILE * file, const char * restrict format)
{
  assert (mesh != NULL);

  for (size_t time_point = 0; time_point < mesh->time_points; ++ time_point)
  {
    for (size_t space_point = 0; space_point < mesh->space_points; ++ space_point)
    {
      fprintf (file, format, mesh_Get (mesh, time_point, space_point));
    }

    if (time_point != (mesh->time_points - 1))
    {
      fprintf (file, "\n");
    }
  }
}


void
mesh_Print_AtTimePoint (const struct Mesh * mesh, FILE * file, const char * restrict format, size_t time_point)
{
  assert (mesh != NULL);

  for (size_t space_point = 0; space_point < mesh->space_points; ++ space_point)
  {
    fprintf (file, format, mesh_Get (mesh, time_point, space_point));
  }
}


typedef real_type temperature_function_type (real_type);


/**
 * @brief ∂u/∂t = α∂²u/∂x²
 * u(x, 0) = f(x)
 * u(0, t) = β₀
 * u(L, t) = β₁
 * t ∈ [0, T]
 * x ∈ [0, L]
 */
struct Parameters
{
  /**
   * @brief α
   */
  real_type diffusivity;

  /**
   * @brief β₀
   */
  real_type boundary_condition_0;

  /**
   * @brief β₁
   */
  real_type boundary_condition_1;

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
parameters_Construct (
  real_type diffusivity, temperature_function_type * initial_condition,
  real_type boundary_condition_0, real_type boundary_condition_1,
  real_type time_max, real_type space_max,
  size_t time_points, size_t space_points
)
{
  assert (initial_condition != NULL);
  assert (time_points > 1);
  assert (space_points > 0);

  const size_t bytes = sizeof (struct Parameters);
  struct Parameters * const new_parameters = malloc (bytes);
  if (new_parameters == NULL)
  {
    fprintf (stderr, "Error: couldn't allocate memory for parameters (%zu bytes).\n", bytes);

    return NULL;
  }

  new_parameters->diffusivity = diffusivity;
  new_parameters->boundary_condition_0 = boundary_condition_0;
  new_parameters->boundary_condition_1 = boundary_condition_1;
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


void
parameters_Print (const struct Parameters * parameters, FILE * output)
{
  assert (output != NULL);

  fprintf (
    output,
    "Parameters{diffusivity:%f;boundary_condition_0:%f;boundary_condition_1:%f;time_max:%f;space_max:%f;time_points:%zu;space_points:%zu}",
    parameters->diffusivity, parameters->boundary_condition_0, parameters->boundary_condition_1, parameters->time_max,
    parameters->space_max, parameters->time_points, parameters->space_points
  );
}


real_type
lerp (real_type x, real_type x_0, real_type x_1, real_type y_0, real_type y_1)
{
  return y_0 + (x - x_0) * (y_1 - y_0) / (x_1 - x_0);
}


typedef int solution_visitor_type (const struct Parameters * parameters, const struct Mesh * mesh, size_t time_point);


int
solve (const struct Parameters * parameters, solution_visitor_type * solution_visitor)
{
  assert (parameters != NULL);
  assert (solution_visitor != NULL);

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

  const real_type time_step = parameters->time_max / (real_type) parameters->time_points;
  const real_type space_step = parameters->space_max / (real_type) parameters->space_points;
  const real_type r = time_step / pow (space_step, 2.0);
  assert (r <= 0.5);
  for (size_t time_point = 1; time_point < parameters->time_points; ++ time_point)
  {
    mesh_Set (mesh, time_point, 0, parameters->boundary_condition_0);
    mesh_Set (mesh, time_point, mesh->space_points - 1, parameters->boundary_condition_1);
    for (size_t space_point = 1; space_point < mesh->space_points - 1; ++ space_point)
    {
      const real_type temperature = (1.0 - 2.0 * r) * mesh_Get (mesh, time_point - 1, space_point)
        + r * mesh_Get (mesh, time_point - 1, space_point - 1)
        + r * mesh_Get (mesh, time_point - 1, space_point + 1);
      mesh_Set (mesh, time_point, space_point, temperature);
    }

    const int visited = solution_visitor (parameters, mesh, time_point);
    if (visited != 0)
    {
      fprintf (stderr, "Error: something went wrong during solving.\n");

      return visited;
    }
  }

  return 0;
}


int
dumpSolution_Stdout (const struct Parameters * parameters, const struct Mesh * mesh, size_t time_point)
{
  fprintf (stdout, "%zu\n", time_point);
  mesh_Print (mesh, stdout, "\t% f");
//  mesh_PrintTimePoint (mesh, stdout, "\t% f", time_point);
  fprintf (stdout, "\n\n");

  return 0;
}


#define FILENAME_BUFFER_LENGTH (128)
#define DUMP_EVERY_NTH (10)


int
dumpSolution_File (const struct Parameters * parameters, const struct Mesh * mesh, size_t time_point)
{
  if ((time_point % DUMP_EVERY_NTH) != 0)
  {
    return 0;
  }

  static char filename[FILENAME_BUFFER_LENGTH];
  sprintf (filename, "%zu.dat", time_point);
  FILE * const output = fopen (filename, "w");
  if (output == NULL)
  {
    fprintf (stderr, "Error: couldn't open output file (%s).\n", filename);

    return - 1;
  }

  fprintf (output, "# ");
  parameters_Print (parameters, output);
  fprintf (output, "\n");
  const real_type time = lerp (
    (real_type) time_point, 0.0, (real_type) (parameters->time_points - 1), 0.0, parameters->time_max
  );
  fprintf (output, "# time_point:%zu;time:%f\n", time_point, time);
  fprintf (output, "# space;temperature\n");

  for (size_t space_point = 0; space_point < parameters->space_points; ++ space_point)
  {
    const real_type space = lerp (
      (real_type) space_point, 0.0, (real_type) (mesh->space_points - 1), 0.0, parameters->space_max
    );
    const real_type temperature = mesh_Get (mesh, time_point, space_point);
    fprintf (output, "%f %f\n", space, temperature);
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
  return sin (space + (0.5 * PI));
}


int
main (int argc, char * argv [])
{
//  const struct Parameters * const parameters = parameters_Construct (1.0, sin, 2.0, 2.0, 10.0, 0.1, 1000, 6);
  const struct Parameters * const parameters = parameters_Construct (1.0, initialCondition, 1.0, - 1.0, 1.0, 5.0 * PI, 1000, 100);
  if (parameters == NULL)
  {
    fprintf (stderr, "Error: couldn't allocate memory for parameters.\n");

    return EXIT_FAILURE;
  }
  
//  const int solved = solve (parameters, & dumpSolution_Stdout);
  const int solved = solve (parameters, & dumpSolution_File);
  if (solved != 0)
  {
    fprintf (stderr, "Error: couldn't solve.\n");
  }

  parameters_Destroy (parameters);

  return EXIT_SUCCESS;
}
