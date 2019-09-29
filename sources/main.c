#include <math.h>  // sin
#include <stddef.h>  // size_t, NULL
#include <stdio.h>  // fprintf_s, stderr
#include <stdlib.h>  // malloc, free, EXIT_FAILURE, EXIT_SUCCESS


#define PI (3.141592653589793238462643383279502884)


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
  const size_t mesh_bytes = sizeof (struct Mesh);
  struct Mesh * const new_mesh = malloc (mesh_bytes);
  if (new_mesh == NULL)
  {
    fprintf_s (stderr, "Error: couldn't allocate memory for mesh (%zu bytes).\n", mesh_bytes);

    return NULL;
  }

  const size_t points_bytes = time_points * space_points * sizeof (real_type);
  new_mesh->points = malloc (points_bytes);
  if (new_mesh->points == NULL)
  {
    fprintf_s (stderr, "Error: couldn't allocate memory for mesh points (%zu bytes).\n", points_bytes);

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
  free (mesh->points);
  free (mesh);
}


size_t
mesh_PointIndex (const struct Mesh * mesh, size_t time_point, size_t space_point)
{
  return (time_point % mesh->time_points) * mesh->space_points + space_point;
}


real_type
mesh_Get (const struct Mesh * mesh, size_t time_point, size_t space_point)
{
  return mesh->points [mesh_PointIndex (mesh, time_point, space_point)];
}


void
mesh_Set (const struct Mesh * mesh, size_t time_point, size_t space_point, real_type new_value)
{
  mesh->points [mesh_PointIndex (mesh, time_point, space_point)] = new_value;
}


void
mesh_Print (const struct Mesh * mesh, FILE * file)
{
  for (size_t time_point = 0; time_point < mesh->time_points; ++ time_point)
  {
    for (size_t space_point = 0; space_point < mesh->space_points; ++ space_point)
    {
      fprintf_s (file, "\t% f", mesh_Get (mesh, time_point, space_point));
    }

    if (time_point != (mesh->time_points - 1))
    {
      fprintf_s (file, "\n");
    }
  }
}


typedef real_type function_type (real_type);


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
   * @brief f(x)
   */
  function_type * initial_condition;

  /**
   * @brief β₀
   */
  real_type boundary_condition_0;

  /**
   * @brief β₁
   */
  real_type boundary_condition_1;

  /**
   * @brief L
   */
  real_type space_max;

  /**
   * @brief T
   */
  real_type time_max;

  size_t space_points;

  size_t time_points;
};


struct Parameters *
parameters_Construct (
  real_type diffusivity, function_type * initial_condition,
  real_type boundary_condition_0, real_type boundary_condition_1,
  real_type space_max, real_type time_max,
  size_t space_points, size_t time_points
)
{
  const size_t bytes = sizeof (struct Parameters);
  struct Parameters * const new_parameters = malloc (bytes);
  if (new_parameters == NULL)
  {
    fprintf_s (stderr, "Error: couldn't allocate memory for parameters (%zu bytes).\n", bytes);

    return NULL;
  }

  new_parameters->diffusivity = diffusivity;
  new_parameters->initial_condition = initial_condition;
  new_parameters->boundary_condition_0 = boundary_condition_0;
  new_parameters->boundary_condition_1 = boundary_condition_1;
  new_parameters->space_max = space_max;
  new_parameters->time_max = time_max;
  new_parameters->space_points = space_points;
  new_parameters->time_points = time_points;

  return new_parameters;
}


void
parameters_Destroy (struct Parameters * parameters)
{
  free (parameters);
}


real_type
lerp (real_type x, real_type x_0, real_type x_1, real_type y_0, real_type y_1)
{
  return y_0 + (x - x_0) * (y_1 - y_0) / (x_1 - x_0);
}


int
solve (const struct Parameters * parameters)
{
  const struct Mesh * const mesh = mesh_Construct (2, parameters->space_points);
  if (mesh == NULL)
  {
    printf_s ("Error: couldn't allocate memory for mesh.\n");

    return - 1;
  }

  mesh_Set (mesh, 0, 0, parameters->boundary_condition_0);
  mesh_Set (mesh, 0, mesh->space_points - 1, parameters->boundary_condition_1);
  for (size_t space_point = 1; space_point < mesh->space_points - 1; ++ space_point)
  {
    const real_type x = lerp (
      (real_type) space_point, 0.0, (real_type) (mesh->space_points - 1), 0.0, parameters->space_max
    );
    const real_type u_x_0 = parameters->initial_condition (x);
    mesh_Set (mesh, 0, space_point, u_x_0);
  }

  for (size_t time_point = 1; time_point < parameters->time_points; ++ time_point)
  {
    mesh_Set (mesh, time_point, 0, parameters->boundary_condition_0);
    mesh_Set (mesh, time_point, mesh->space_points - 1, parameters->boundary_condition_1);
    for (size_t space_point = 1; space_point < mesh->space_points - 1; ++ space_point)
    {
//      const real_type x
    }
  }

  mesh_Print (mesh, stdout);
  fprintf_s (stdout, "\n");

  return 0;
}


int
main (int argc, char * argv [])
{
  const struct Parameters * const parameters = parameters_Construct (
    1.0, sin, 0.0, 0.0, 2.0 * PI, 10.0, 5, 10
  );
  if (parameters == NULL)
  {
    printf_s ("Error: couldn't allocate memory for mesh.\n");

    return EXIT_FAILURE;
  }

  const int solved = solve (parameters);
  if (solved != 0)
  {
    fprintf_s (stderr, "Error: couldn't solve.\n");
  }

  parameters_Destroy (parameters);

  return EXIT_SUCCESS;
}
