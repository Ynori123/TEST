#ifndef ROOTIO_HH
#define ROOTIO_HH

#include <TFile.h>
#include <TTree.h>
#include <string>
#include <array>

class RootIO {
public:
    RootIO();
    ~RootIO();

    void OpenFile(const std::string& filename);
    void CloseFile();

    void SetRunID(int runID) { fRunID = runID; }

    // 建树
    void CreateStepTree();
    void CreateEventTree();

    // 每步写一条
    void AddStep(int eventID, int trackID, int stepNo, int pdg,
                 float x, float y, float z,
                 float px, float py, float pz,
                 float ekin, float time, float stepLen,
                 const std::string& volume, const std::string& material, const std::string& process,
                 float edep);

    // 每事件写一条
    void FillEvent(int eventID, double totalEnergy,
                   double wposX, double wposY, double wposZ);

    // 可选显式写盘
    void WriteStepTree();
    void WriteEventTree();

    // 新增：深度分布树创建与填充接口
    void CreateDepthTrees();
    void AddDepthEvent(const double* binEdep);        // 每事件分布
    void AccumulateBinSum(const double* binEdep);     // 累计到总和
    void FillDepthMean(int nEvents);                  // 计算均值并写入
    // static constexpr int kNBins = 4;                  // 4 个 15cm 薄层
    static constexpr int kNBins = 60;                  // 薄层 60cm
    static constexpr double kZMin = -30.0;            // cm
    static constexpr double kZMax =  30.0;            // cm
    static constexpr double kBinWidth = (kZMax - kZMin) / kNBins; //  cm

private:
    // 文件与树
    TFile* fFile = nullptr;
    TTree* fStepTree = nullptr;
    TTree* fEventTree = nullptr;
    TTree* fDepthEventTree = nullptr;
    TTree* fDepthMeanTree  = nullptr;

    // Run 信息
    int fRunID = -1;

    // stepTree 分支变量（每步）
    int   s_runID = -1, s_eventID = -1, s_trackID = 0, s_stepNo = 0, s_pdg = 0;
    float s_x = 0.f, s_y = 0.f, s_z = 0.f;
    float s_px = 0.f, s_py = 0.f, s_pz = 0.f;
    float s_ekin = 0.f, s_time = 0.f, s_stepLen = 0.f, s_edep = 0.f;

    // 修改：使用固定长度字符数组代替 std::string，避免旧树复用时对象指针失效
    static constexpr int kStrMax = 128;
    char s_volume[kStrMax]{};
    char s_material[kStrMax]{};
    char s_process[kStrMax]{};

    // eventTree 分支变量（每事件）
    int    e_runID = -1, e_eventID = -1;
    double e_totalEnergy = 0.;
    double e_wposX = 0., e_wposY = 0., e_wposZ = 0.;

    // 深度事件级分支
    int    d_runID = -1, d_eventID = -1;
    double d_binEdep[kNBins]{}; // 每事件各薄层能量（MeV）

    // 深度均值分支（每 run 一条）
    int    m_runID = -1, m_nEvents = 0;
    double m_binMean[kNBins]{};     // <E_dep(z_i)>
    double m_dEdz[kNBins]{};        // -dE/dz ≈ <E_dep>/Δz
    std::array<double, kNBins> fBinSum{}; // 运行内累计总和

    // 新增：为已存在的树重新绑定分支地址
    void AttachStepBranches();
    void AttachEventBranches();
    // 新增：附着深度树分支
    void AttachDepthEventBranches();
    void AttachDepthMeanBranches();
};

#endif // ROOTIO_HH
