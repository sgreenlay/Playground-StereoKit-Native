#include <stereokit.h>
#include <stereokit_ui.h>

#include <JSCRuntime.h>
#include <jsi/jsi.h>

#include <random>

#include <android/log.h>

using namespace sk;
using namespace facebook::jsi;
using namespace facebook::jsc;

mesh_t mesh;
material_t mat;
std::unique_ptr<Runtime> rt;

// https://codereview.stackexchange.com/questions/261326/custom-uuid-implementation-on-c
static std::string generate_uuid(size_t len)
{
    static const char x[] = "0123456789abcdef";

    std::string uuid;
    uuid.reserve(len);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dis(0, sizeof(x) - 2);
    for (size_t i = 0; i < len; i++)
        uuid += x[dis(gen)];

    return uuid;
}

class SKCube : public HostObject {
    Value get(Runtime& rt, const PropNameID& sym) override {
        return Value::undefined();
    }

    void set(Runtime& rt, const PropNameID& sym, const Value& val) override {
        auto propertyName = sym.utf8(rt);
        if (propertyName == "x") {
            pose.position.x = val.getNumber();
        } else if (propertyName == "y") {
            pose.position.y = val.getNumber();
        } else if (propertyName == "z") {
            pose.position.z = val.getNumber();
        }
    }

public:
    std::string id = generate_uuid(14);
    pose_t pose = {vec3{0, 0, 0}, quat_identity};
};

std::vector<std::shared_ptr<SKCube>> cubes;

class SKBridgeObject : public HostObject {
    Value get(Runtime& rt, const PropNameID& sym) override {
        auto propertyName = sym.utf8(rt);
        if (propertyName == "createCube") {
            return Function::createFromHostFunction(
                rt,
                sym,
                0,
                [](Runtime &rt,
                    Value const &thisValue,
                    Value const *arguments,
                    size_t count) noexcept -> Value {
                    auto cube = std::make_shared<SKCube>();
                    cubes.push_back(cube);
                    return Object::createFromHostObject(rt, cube);
                }
            );
        }
        return Value::undefined();
    }

    void set(Runtime&, const PropNameID&, const Value&) override {
        // TODO
    }
};

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

    auto bridge = Object::createFromHostObject(*rt, std::make_shared<SKBridgeObject>());
    rt->global().setProperty(*rt, "stereokit", bridge);

    const char * script1 = R"(
        var a = stereokit.createCube();
        a.z = -2.0;

        var b = stereokit.createCube();
        b.x = -1.2;
        b.z = -2.0;

        var c = stereokit.createCube();
        c.x = 1.2;
        c.z = -2.0;
    )";
    rt->evaluateJavaScript(std::make_unique<StringBuffer>(script1), "");

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
        for (auto&& cube : cubes) {
            ui_handle_begin(cube->id.c_str(), cube->pose, mesh_get_bounds(mesh), false);
            render_add_mesh(mesh, mat, matrix_identity);
            ui_handle_end();
        }
    });
}

void app_exit()
{
    sk_shutdown();
}
