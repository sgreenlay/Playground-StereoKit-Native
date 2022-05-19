#include <stereokit.h>
#include <stereokit_ui.h>

#include <JSCRuntime.h>
#include <jsi/jsi.h>

#include <android/log.h>

using namespace sk;
using namespace facebook::jsi;
using namespace facebook::jsc;

mesh_t mesh;
material_t mat;
pose_t pose = {vec3{0, 0, -2.0f}, quat_identity};
std::unique_ptr<Runtime> rt;

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

    mesh = mesh_find(default_id_mesh_cube);
    mat = material_find(default_id_material);

    rt = makeJSCRuntime();

    const char * script = R""""(
        var str = "";
        for (var i = 0; i < 10; ++i) {
            str += i + " ";
        }
        str
    )"""";
    auto v = rt->evaluateJavaScript(std::make_unique<StringBuffer>(script), "");
    auto result = v.getString(*rt).utf8(*rt);
    __android_log_print(ANDROID_LOG_DEBUG, "SKPlayground", "JSResult: %s", result.c_str());

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
        ui_handle_begin("Object", pose, mesh_get_bounds(mesh), false);
        render_add_mesh(mesh, mat, matrix_identity);
        ui_handle_end();
    });
}

void app_exit()
{
    sk_shutdown();
}
