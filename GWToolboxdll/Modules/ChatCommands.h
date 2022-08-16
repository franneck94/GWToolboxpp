#pragma once

#include <GWCA/Constants/Constants.h>
#include <GWCA/Utilities/Hook.h>

#include <GWCA/GameContainers/GamePos.h>

#include <GWCA/Managers/UIMgr.h>
#include <GWCA/Managers/SkillbarMgr.h>

#include <Utils/GuiUtils.h>

#include <ToolboxModule.h>
#include <ToolboxUIElement.h>

class ChatCommands : public ToolboxModule {
    const float DEFAULT_CAM_SPEED = 1000.f;                      // 600 units per sec
    const float ROTATION_SPEED = static_cast<float>(M_PI) / 3.f; // 6 seconds for full rotation

    ChatCommands();
    ~ChatCommands();

public:
    static ChatCommands& Instance() {
        static ChatCommands instance;
        return instance;
    }
    struct PendingTransmo {
        PendingTransmo(
            DWORD _npcid = 0, DWORD _scale = 0x64000000, DWORD _npcmfid = 0, DWORD _npcmfd = 0, DWORD _flags = 0)
            : npc_id(_npcid)
            , scale(_scale)
            , npc_model_file_id(_npcmfid)
            , npc_model_file_data(_npcmfd)
            , flags(_flags){};
        DWORD npc_id = 0;
        DWORD scale = 0x64000000;
        DWORD npc_model_file_id = 0;
        DWORD npc_model_file_data = 0;
        DWORD flags = 0;
    };

    enum TargetType : uint32_t { Gadget = 0, Player, Npc, Item, Living, Enemy, Ally };

    const char* Name() const override { return "Chat Commands"; }
    const char* SettingsName() const override { return "Game Settings"; }

    void Initialize() override;
    void LoadSettings(CSimpleIni* ini) override;
    void SaveSettings(CSimpleIni* ini) override;
    void DrawSettingInternal() override;

    void DrawHelp() override;

    bool WndProc(UINT Message, WPARAM wParam, LPARAM lParam);

    // Update. Will always be called every frame.
    void Update(float delta) override;

    static void CmdReapplyTitle(const wchar_t* message, int argc, LPWSTR* argv);

private:
    static bool ReadTemplateFile(std::wstring path, char* buff, size_t buffSize);

    static bool IsLuxon();

    static void CmdEnterMission(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdAge2(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdDialog(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdTB(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdDamage(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdObserverReset(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdChest(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdAfk(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdTarget(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdUseSkill(const wchar_t *, int argc, LPWSTR *argv);
    static void CmdShow(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdHide(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdToggle(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdZoom(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdCamera(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdSCWiki(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdLoad(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdPing(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdTransmo(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdResize(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdPingEquipment(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdTransmoTarget(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdTransmoParty(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdTransmoAgent(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdHeroBehaviour(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdPingQuest(const wchar_t* message, int argc, LPWSTR* argv);
    static void CmdMorale(const wchar_t*, int argc, LPWSTR* argv);
    static void CmdVolume(const wchar_t*, int argc, LPWSTR* argv);
    static void CmdSetHardMode(const wchar_t*, int argc, LPWSTR* argv);
    static void CmdSetNormalMode(const wchar_t*, int argc, LPWSTR* argv);
    static void CmdAnimation(const wchar_t*, int argc, LPWSTR* argv);
    static void CmdMute(const wchar_t*, int argc, LPWSTR* argv);
    // Trigger hall of monuments info for current target or given player name
    static void CmdHom(const wchar_t*, int argc, LPWSTR* argv);

    static void TransmoAgent(DWORD agent_id, PendingTransmo& transmo);
    static bool GetNPCInfoByName(const std::string name, PendingTransmo& transmo);
    static bool GetNPCInfoByName(const std::wstring name, PendingTransmo& transmo);
    static bool ParseScale(int scale, PendingTransmo& transmo);
    static bool GetTargetTransmoInfo(PendingTransmo& transmo);
    static void TargetNearest(const wchar_t* model_id_or_name, TargetType type);
    static const wchar_t* GetRemainingArgsWstr(const wchar_t* message, int argc_start);

    static std::vector<ToolboxUIElement*> MatchingWindows(const wchar_t* message, int argc, LPWSTR* argv);
    static GW::UI::WindowID MatchingGWWindow(const wchar_t*, int argc, LPWSTR* argv);

    float cam_speed = DEFAULT_CAM_SPEED;
    bool forward_fix_z = true;
    uint32_t default_title_id;

    struct SearchAgent {
        clock_t started = 0;
        std::vector<std::pair<uint32_t, GuiUtils::EncString*>> npc_names;
        std::wstring search;
        void Init(const wchar_t* search, TargetType type = Npc);
        void Update();
        ~SearchAgent() {
            for (const auto& it : npc_names)
                delete it.second;
            npc_names.clear();
        }
    } npc_to_find;

    struct BaseUseSkill {
        uint32_t slot = 0; // 1-8 range
        float skill_usage_delay = 0.0F;
        clock_t skill_timer = clock();

        virtual void Update() = 0;
        void CastSelectedSkill(
            const uint32_t current_energy, const GW::Skillbar* skillbar, const uint32_t target_id = 0);
    };

    struct UseSkill : public BaseUseSkill {
        void Update() override;
    };
    UseSkill useskill;

    struct QuestPing {
        GuiUtils::EncString name;
        GuiUtils::EncString description;
        GuiUtils::EncString objectives;
        void Update();
        void Init();
    } quest_ping;

protected:
    float SettingsWeighting() override { return 1.2f; };
};
