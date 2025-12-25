
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
// 物理列表头文件
#include "FTFP_BERT.hh"
#include <unistd.h>
#include <filesystem> // 用于路径处理

int main(int argc, char **argv)
{
    // 创建运行管理器
    G4RunManager *runManager = new G4RunManager();

    // 设置必需的初始化类
    runManager->SetUserInitialization(new DetectorConstruction());
    runManager->SetUserInitialization(new FTFP_BERT()); // 设置物理过程
    runManager->SetUserInitialization(new ActionInitialization());

    // 初始化 G4 内核
    runManager->Initialize();

    // 初始化可视化管理器
    G4VisExecutive *visManager = new G4VisExecutive();
    visManager->Initialize();

    // 获取 UI 管理器
    G4UImanager *UImanager = G4UImanager::GetUIpointer();
    // 按粒子种类设置颜色
    UImanager->ApplyCommand("/vis/modeling/trajectories/create/drawByParticleID");
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set e- blue");
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set e+ red");
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set gamma green");
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set proton cyan");
    // 更多粒子颜色（使用名称或RGB）
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set anti_proton 1 0.6 0"); // 橙
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set neutron 1 0.84 0");    // 金黄（中子）
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set mu- magenta");
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set mu+ violet");
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set pi- 0.6 0.3 0.0"); // 棕
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set pi+ 1 0.5 0");     // 橙
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set pi0 grey");
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set kaon- 0 0.5 0.5");  // 青绿
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set kaon+ 0 0.5 0");    // 深绿
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set deuteron 0.5 1 1"); // 浅青
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set triton 0.3 0.8 0.8");
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set alpha 1 0.7 0.2"); // 金橙
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set He3 0.8 0.5 0.2");
    UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set genericIon 0.6 0 0.6"); // 紫

    // 获取当前工作目录并构建宏文件路径
    char cwd[4096];
    std::string macroDir;
    if (getcwd(cwd, sizeof(cwd)))
    {
        macroDir = std::string(cwd) + "/../macros/";
        G4cout << "当前工作目录: " << cwd << G4endl;
        G4cout << "宏文件目录: " << macroDir << G4endl;
    }

    // 如果没有参数，启动交互式会话
    if (argc == 1)
    {
        G4UIExecutive *ui = nullptr;
        try
        {
            ui = new G4UIExecutive(argc, argv);

            // 检查宏文件是否存在
            std::string initMacroPath = macroDir + "init.mac";
            if (!std::filesystem::exists(initMacroPath))
            {
                G4cerr << "错误: 找不到宏文件: " << initMacroPath << G4endl;
                G4cerr << "请确保在正确目录运行程序或提供宏文件路径参数" << G4endl;

                // 尝试使用默认可视化命令
                UImanager->ApplyCommand("/vis/open OGL 600x600-0+0");
                UImanager->ApplyCommand("/vis/drawVolume");
                UImanager->ApplyCommand("/vis/viewer/set/autoRefresh true");
                UImanager->ApplyCommand("/vis/scene/add/trajectories smooth");
                UImanager->ApplyCommand("/vis/scene/endOfEventAction accumulate");
            }
            else
            {
                // 设置宏文件路径并执行初始化宏
                UImanager->ApplyCommand("/control/macroPath " + macroDir);
                UImanager->ApplyCommand("/control/execute init.mac");
            }

            ui->SessionStart();
            delete ui;
        }
        catch (...)
        {
            if (ui)
                delete ui;
            G4cerr << "创建交互式会话失败" << G4endl;

            // 提供非交互式替代方案
            G4cout << "使用命令行模式..." << G4endl;
            UImanager->ApplyCommand("/run/verbose 1");
            UImanager->ApplyCommand("/event/verbose 0");
            UImanager->ApplyCommand("/tracking/verbose 0");
            UImanager->ApplyCommand("/run/beamOn 10");
        }
    }
    else
    {
        // 如果有参数，执行指定的宏文件
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
    }

    // 清理资源
    delete visManager;
    delete runManager;

    // 输出保存的 ROOT 文件路径
    if (getcwd(cwd, sizeof(cwd)))
    {
        G4cout << "ROOT 输出文件路径: " << cwd << "/output.root" << G4endl;
    }
    else
    {
        G4cout << "ROOT 输出文件名: output.root" << G4endl;
    }

    return 0;
}

// git test
// try to rest now