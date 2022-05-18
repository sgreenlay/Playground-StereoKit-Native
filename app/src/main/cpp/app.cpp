#include <stereokit.h>
#include <stereokit_ui.h>

using namespace sk;

mesh_t sphere_mesh;
material_t sphere_mat;
pose_t sphere_pose = {vec3{0, 0, -2.0f}, quat_identity};

bool app_init(struct android_app *state)
{
    log_set_filter(log_diagnostic);

    sk_settings_t settings = {};

    settings.app_name = "SKPlayground";
    settings.android_activity = state->activity->clazz;
    settings.android_java_vm = state->activity->vm;
    settings.display_preference = display_mode_mixedreality;

    if (!sk_init(settings))
    {
        return false;
    }

    sphere_mesh = mesh_find(default_id_mesh_sphere);
    sphere_mat = material_find(default_id_material);

    return true;
}

void app_handle_cmd(android_app *evt_app, int32_t cmd)
{
    switch (cmd)
    {
    case APP_CMD_INIT_WINDOW:
    case APP_CMD_WINDOW_RESIZED:
    case APP_CMD_CONFIG_CHANGED:
        sk_set_window(evt_app->window);
        break;
    case APP_CMD_TERM_WINDOW:
        sk_set_window(nullptr);
        break;
    }
}

void app_step()
{
    sk_step([]() {
        ui_handle_begin("Sphere", sphere_pose, mesh_get_bounds(sphere_mesh), false);
        render_add_mesh(sphere_mesh, sphere_mat, matrix_identity);
        ui_handle_end();
    });
}

void app_exit()
{
    sk_shutdown();
}
