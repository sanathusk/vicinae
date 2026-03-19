#include "actions/app/app-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "services/window-manager/window-manager.hpp"
#include "ui/action-pannel/action.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"

void OpenInTerminalAction::execute(ApplicationContext *ctx) {
  auto appDb = ctx->services->appDb();
  auto toast = ctx->services->toastService();

  m_opts.emulator = m_emulator.get();

  if (!appDb->launchTerminalCommand(m_args, m_opts)) {
    toast->setToast("Failed to start app", ToastStyle::Danger);
    return;
  }

  ctx->navigation->closeWindow();
  if (m_clearSearch) ctx->navigation->clearSearchText();
}

OpenInTerminalAction::OpenInTerminalAction(const std::shared_ptr<AbstractApplication> &emulator,
                                           const std::vector<QString> &cmdline,
                                           const LaunchTerminalCommandOptions &opts)
    : m_emulator(emulator), m_args(cmdline), m_opts(opts) {}

OpenAppAction::OpenAppAction(const std::shared_ptr<AbstractApplication> &app, const QString &title,
                             const std::vector<QString> &args, bool forceNewInstance)
    : AbstractAction(title, app->icon()), application(app), args(args), m_forceNewInstance(forceNewInstance) {}

void OpenAppAction::execute(ApplicationContext *ctx) {
  auto appDb = ctx->services->appDb();
  auto wm = ctx->services->windowManager();

  if (!m_forceNewInstance) {
    auto activeWindows = wm->findAppWindows(*application);
    if (!activeWindows.empty()) {
      wm->provider()->focusWindowSync(*activeWindows.front().get());
      ctx->navigation->closeWindow();
      if (m_clearSearch) ctx->navigation->clearSearchText();
      return;
    }
  }

  auto toast = ctx->services->toastService();

  if (!appDb->launch(*application, args)) {
    toast->setToast("Failed to start app", ToastStyle::Danger);
    return;
  }

  ctx->navigation->closeWindow();
  if (m_clearSearch) ctx->navigation->clearSearchText();
}

OpenRawProgramAction::OpenRawProgramAction(const std::vector<QString> &args) : m_args(args) {}

void OpenInBrowserAction::execute(ApplicationContext *ctx) {
  const auto toast = ctx->services->toastService();

  if (!ctx->services->appDb()->openTarget(m_url)) {
    toast->failure("Failed to open toast");
    return;
  }

  ctx->navigation->showHud("Opened in browser");
}
