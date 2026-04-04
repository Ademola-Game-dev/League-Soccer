#include "careerpages.hpp"

#include "../../data/teamdata.hpp"
#include "../../data/playerdata.hpp"
#include "../../gamedefines.hpp"
#include "../../main.hpp"
#include "../pagefactory.hpp"
#include "base/properties.hpp"
#include "base/utils.hpp"
#include "career_database.hpp"
#include "utils/gui2/widgets/frame.hpp"
#include "utils/gui2/widgets/image.hpp"
#include <algorithm>
#include <cstdio>

using namespace blunted;

namespace {

constexpr const char* kCareerBackdropPath = "media/menu/backgrounds/megabackground01.jpg";
const Vector3 kCareerMutedText(165, 170, 180);

Gui2Image* AddCareerBackdrop(Gui2Page* page, Gui2WindowManager* windowManager,
                             const std::string& name) {
  Gui2Image* backdrop = new Gui2Image(windowManager, name, 0, 0, 100, 100);
  backdrop->LoadImage(kCareerBackdropPath);
  backdrop->SetZoom(1.08f, 1.08f);
  backdrop->SetZPriority(-20);
  page->AddView(backdrop);
  backdrop->Show();

  Gui2Image* scrim = new Gui2Image(windowManager, name + "_scrim", 0, 0, 100, 100);
  const int scrimW = static_cast<int>(scrim->GetImage2D()->GetSize().coords[0]);
  const int scrimH = static_cast<int>(scrim->GetImage2D()->GetSize().coords[1]);
  scrim->GetImage2D()->DrawRectangle(0, 0, scrimW, scrimH,
                                     Vector3(0, 0, 0), 120);
  scrim->GetImage2D()->OnChange();
  scrim->SetZPriority(-19);
  page->AddView(scrim);
  scrim->Show();

  return backdrop;
}

Gui2Frame* AddCareerPanel(Gui2Page* page, Gui2WindowManager* windowManager,
                          const std::string& name, float x, float y, float w, float h) {
  Gui2Frame* panel = new Gui2Frame(windowManager, name, x, y, w, h, true);
  page->AddView(panel);
  panel->Show();

  Gui2Image* fill = new Gui2Image(windowManager, name + "_fill", 0, 0, w, h);
  const int fillW = static_cast<int>(fill->GetImage2D()->GetSize().coords[0]);
  const int fillH = static_cast<int>(fill->GetImage2D()->GetSize().coords[1]);
  fill->GetImage2D()->DrawRectangle(0, 0, fillW, fillH,
                                    Vector3(8, 10, 14), 235);
  fill->GetImage2D()->DrawRectangle(0, 0, 3, fillH,
                                    windowManager->GetStyle()->GetColor(e_DecorationType_Bright2),
                                    160);
  fill->GetImage2D()->OnChange();
  panel->AddView(fill);
  fill->Show();

  return panel;
}

Gui2Caption* AddCareerCaption(Gui2View* parent, Gui2WindowManager* windowManager,
                              const std::string& name, float x, float y, float w, float h,
                              const std::string& text, const Vector3* color = nullptr) {
  Gui2Caption* caption = new Gui2Caption(windowManager, name, x, y, w, h, text);
  if (color) {
    caption->SetColor(*color);
  }
  parent->AddView(caption);
  caption->Show();
  return caption;
}

std::string GetCareerModeTitle(const std::string& mode) {
  if (mode == "owner" || mode == "mygm" || mode == "manager") return "Owner Career";
  if (mode == "player") return "Player Career";
  if (mode == "mycoach") return "Coach Career";
  return "Owner Career";
}

std::string GetCareerModePitchLine1(const std::string& mode) {
  if (mode == "owner" || mode == "mygm" || mode == "manager") return "Run the club from";
  if (mode == "player") return "Develop from prospect";
  if (mode == "mycoach") return "Own the touchline and";
  return "Run the club from";
}

std::string GetCareerModePitchLine2(const std::string& mode) {
  if (mode == "owner" || mode == "mygm" || mode == "manager") return "budgets to recruitment.";
  if (mode == "player") return "to first-team leader.";
  if (mode == "mycoach") return "shape the dressing room.";
  return "budgets to recruitment.";
}

std::string FormatMoney(long long amount) {
  return "EUR " + std::to_string(amount);
}

}  // namespace

// ---------------------------------------------------------------------------
// CareerMenuPage
// ---------------------------------------------------------------------------

CareerMenuPage::CareerMenuPage(Gui2WindowManager* windowManager, const Gui2PageData& pageData)
    : Gui2Page(windowManager, pageData) {
  AddCareerBackdrop(this, windowManager, "career_menu_backdrop");

  Gui2Frame* heroPanel =
      AddCareerPanel(this, windowManager, "career_menu_hero_panel", 6, 12, 42, 76);
  Gui2Frame* optionsPanel =
      AddCareerPanel(this, windowManager, "career_menu_options_panel", 54, 12, 40, 76);

  const Vector3 accentColor =
      windowManager->GetStyle()->GetColor(e_DecorationType_Bright2);
  AddCareerCaption(heroPanel, windowManager, "caption_career_eyebrow", 6, 8, 24, 3,
                   "Career", &accentColor);
  AddCareerCaption(heroPanel, windowManager, "caption_career_title_1", 6, 18, 30, 5,
                   "Build Your");
  AddCareerCaption(heroPanel, windowManager, "caption_career_title_2", 6, 25, 34, 5,
                   "Club Legacy");
  AddCareerCaption(heroPanel, windowManager, "caption_career_copy_1", 6, 38, 30, 3,
                   "Choose a role.");
  AddCareerCaption(heroPanel, windowManager, "caption_career_copy_2", 6, 43, 34, 3,
                   "Shape a squad identity.");
  AddCareerCaption(heroPanel, windowManager, "caption_career_copy_3", 6, 48, 30, 3,
                   "Drive every season.");
  AddCareerCaption(heroPanel, windowManager, "caption_career_copy_4", 6, 60, 30, 2.5f,
                   "From first signing to final");
  AddCareerCaption(heroPanel, windowManager, "caption_career_copy_5", 6, 64, 26, 2.5f,
                   "matchday, this is your club.", &kCareerMutedText);

  AddCareerCaption(optionsPanel, windowManager, "caption_career_options", 8, 8, 24, 3,
                   "Select Role", &accentColor);
  AddCareerCaption(optionsPanel, windowManager, "caption_career_options_sub", 8, 14, 24, 2.5f,
                   "Choose your starting path.", &kCareerMutedText);

  Gui2Button* btnOwner = new Gui2Button(windowManager, "btn_ownercareer", 0, 0, 30, 5, "Owner Career");
  Gui2Button* btnPlayer =
      new Gui2Button(windowManager, "btn_playercareer", 0, 0, 30, 5, "Player Career");

  btnOwner->sig_OnClick.connect([this](...) { GoOwnerCareer(); });
  btnPlayer->sig_OnClick.connect([this](...) { GoPlayerCareer(); });

  Gui2Grid* grid = new Gui2Grid(windowManager, "career_grid", 5, 22, 30, 56);
  grid->AddView(btnOwner, 0, 0);
  grid->AddView(btnPlayer, 1, 0);
  grid->UpdateLayout(0.8f, 0.8f, 1.1f, 1.1f);

  optionsPanel->AddView(grid);
  grid->Show();

  AddCareerCaption(optionsPanel, windowManager, "caption_career_footer", 8, 82, 26, 2.5f,
                   "Two career paths.", &kCareerMutedText);

  btnOwner->SetFocus();
  this->Show();
}

CareerMenuPage::~CareerMenuPage() {}

void CareerMenuPage::GoCareerMode(const std::string& mode) {
  Properties props;
  props.Set("careerMode", mode);
  CreatePage(e_PageID_CareerNewGame, props);
}

void CareerMenuPage::GoOwnerCareer() {
  GoCareerMode("owner");
}
void CareerMenuPage::GoPlayerCareer() {
  GoCareerMode("player");
}

// ---------------------------------------------------------------------------
// CareerNewGamePage
// ---------------------------------------------------------------------------

CareerNewGamePage::CareerNewGamePage(Gui2WindowManager* windowManager, const Gui2PageData& pageData)
    : Gui2Page(windowManager, pageData) {
  m_mode = pageData.properties ? pageData.properties->Get("careerMode", "owner") : "owner";

  AddCareerBackdrop(this, windowManager, "career_new_backdrop");

  const Vector3 accentColor =
      windowManager->GetStyle()->GetColor(e_DecorationType_Bright2);
  const std::string modeLabel = GetCareerModeTitle(m_mode);

  Gui2Frame* summaryPanel =
      AddCareerPanel(this, windowManager, "career_new_summary_panel", 6, 12, 34, 76);
  Gui2Frame* setupPanel =
      AddCareerPanel(this, windowManager, "career_new_setup_panel", 44, 12, 50, 76);

  AddCareerCaption(summaryPanel, windowManager, "caption_newgame_tag", 8, 8, 22, 3,
                   "New Career", &accentColor);
  AddCareerCaption(summaryPanel, windowManager, "caption_newgame_mode", 8, 18, 24, 4,
                   modeLabel);
  AddCareerCaption(summaryPanel, windowManager, "caption_newgame_pitch_1", 8, 32, 24, 3,
                   GetCareerModePitchLine1(m_mode));
  AddCareerCaption(summaryPanel, windowManager, "caption_newgame_pitch_2", 8, 37, 24, 3,
                   GetCareerModePitchLine2(m_mode));
  AddCareerCaption(summaryPanel, windowManager, "caption_newgame_notes_1", 8, 52, 24, 2.5f,
                   "Choose a club, name your");
  AddCareerCaption(summaryPanel, windowManager, "caption_newgame_notes_2", 8, 56, 24, 2.5f,
                   "career, and step straight");
  AddCareerCaption(summaryPanel, windowManager, "caption_newgame_notes_3", 8, 60, 24, 2.5f,
                   "into the hub.", &kCareerMutedText);

  AddCareerCaption(setupPanel, windowManager, "caption_newgame_setup", 6, 8, 20, 3,
                   "Club Setup", &accentColor);

  Gui2Caption* teamCaption =
      new Gui2Caption(windowManager, "caption_newgame_team", 6, 20, 20, 2.5f, "Select your team:");
  setupPanel->AddView(teamCaption);
  teamCaption->Show();

  teamSelectPulldown =
      new Gui2Pulldown(windowManager, "pulldown_career_teamselect", 6, 25, 38, 3.5f);
  RefreshTeamSelect();
  teamSelectPulldown->sig_OnChange.connect([this](Gui2Pulldown* pd) {
    m_selectedTeamID = pd->GetSelected();
  });
  setupPanel->AddView(teamSelectPulldown);
  teamSelectPulldown->Show();

  std::string nameFieldLabel = "Owner name:";
  std::string nameDefault = "Owner";
  if (m_mode == "player") {
    nameFieldLabel = "Player name:";
    nameDefault = "Player";
  }

  Gui2Caption* mgrCaption =
      new Gui2Caption(windowManager, "caption_newgame_mgr", 6, 38, 20, 2.5f, nameFieldLabel);
  setupPanel->AddView(mgrCaption);
  mgrCaption->Show();

  managerNameInput =
      new Gui2EditLine(windowManager, "editline_career_mgrname", 6, 43, 38, 3.5f, nameDefault);
  managerNameInput->SetMaxLength(32);
  setupPanel->AddView(managerNameInput);
  managerNameInput->Show();

  Gui2Button* btnStart =
      new Gui2Button(windowManager, "btn_start_career", 6, 64, 38, 4.5f, "Start Career");
  btnStart->sig_OnClick.connect([this](...) { StartCareer(); });
  setupPanel->AddView(btnStart);
  btnStart->Show();

  Gui2Button* btnBack =
      new Gui2Button(windowManager, "btn_newgame_back", 6, 72, 38, 4.0f, "Back to Career Select");
  btnBack->sig_OnClick.connect([this](...) { CreatePage(e_PageID_CareerMenu); });
  setupPanel->AddView(btnBack);
  btnBack->Show();

  teamSelectPulldown->SetFocus();

  this->Show();
}

CareerNewGamePage::~CareerNewGamePage() {}

void CareerNewGamePage::RefreshTeamSelect() {
  teamSelectPulldown->ClearEntries();
  try {
    auto result = GetDB()->Query(
        "SELECT teams.id, teams.name, leagues.name FROM teams "
        "JOIN leagues ON teams.league_id = leagues.id ORDER BY leagues.name, teams.name");
    for (unsigned int r = 0; r < result->data.size(); r++) {
      std::string id = result->data.at(r).at(0);
      std::string teamName = result->data.at(r).at(1);
      std::string leagueName = result->data.at(r).at(2);
      teamSelectPulldown->AddEntry(teamName + " (" + leagueName + ")", id);
    }
  } catch (...) {
    teamSelectPulldown->AddEntry("No teams found", "0");
  }
  teamSelectPulldown->SetSelected(0);
  m_selectedTeamID = teamSelectPulldown->GetSelected();
}

static std::string RoleToCareerPos(e_PlayerRole role) {
  return GetRoleName(role);
}

static int ComputePlayerOVR(PlayerData* pd) {
  const char* statNames[] = {
    "physical_balance", "physical_reaction", "physical_acceleration", "physical_velocity",
    "physical_stamina", "physical_agility", "physical_shotpower",
    "technical_standingtackle", "technical_slidingtackle", "technical_ballcontrol",
    "technical_dribble", "technical_shortpass", "technical_highpass", "technical_header",
    "technical_shot", "technical_volley",
    "mental_calmness", "mental_workrate", "mental_resilience",
    "mental_defensivepositioning", "mental_offensivepositioning", "mental_vision"
  };
  float total = 0.0f;
  int count = 0;
  for (const char* name : statNames) {
    total += pd->GetStat(name);
    count++;
  }
  return count > 0 ? static_cast<int>((total / count) * 100.0f) : 50;
}

void CareerNewGamePage::StartCareer() {
  int teamDBID = atoi(m_selectedTeamID.c_str());

  std::string teamName = "Unknown";
  std::string leagueName = "Unknown";
  try {
    auto result = GetDB()->Query(
        "SELECT teams.name, leagues.name FROM teams "
        "JOIN leagues ON teams.league_id = leagues.id WHERE teams.id = " +
        int_to_str(teamDBID));
    if (!result->data.empty()) {
      teamName = result->data.at(0).at(0);
      leagueName = result->data.at(0).at(1);
    }
  } catch (...) {}

  CareerDatabase::GetInstance().Initialize("user/career");
  CareerDatabase::GetInstance().CreateNewCareer(
      teamName, m_mode, managerNameInput->GetText());

  CareerSave* save = CareerDatabase::GetInstance().GetActiveSave();
  if (save) {
    save->teamID = teamDBID;
    save->leagueName = leagueName;

    save->roster.clear();
    TeamData teamData(teamDBID);
    const auto& players = teamData.GetPlayerData();
    for (const auto& pd : players) {
      int ovr = ComputePlayerOVR(pd.get());
      int pot = std::min(99, ovr + static_cast<int>(random(3, 20)));
      int age = 22;
      try {
        auto ageResult = GetDB()->Query(
            "SELECT age FROM players WHERE id = " + int_to_str(pd->GetDatabaseID()));
        if (!ageResult->data.empty()) {
          age = atoi(ageResult->data.at(0).at(0).c_str());
          pot = std::min(99, ovr + static_cast<int>((99 - age) * 0.5));
        }
      } catch (...) {}

      const auto& roles = pd->GetRoles();
      std::string pos = roles.empty() ? "CM" : RoleToCareerPos(roles[0]);

      long long value = static_cast<long long>(ovr) * static_cast<long long>(ovr) * 5000;
      long long wage = (value / 1000) + static_cast<int>(random(500, 2000));

      CareerPlayer cp(
          pd->GetFirstName() + " " + pd->GetLastName(),
          ovr, pot, age, value, wage, pos);
      cp.databaseID = pd->GetDatabaseID();
      cp.contractYearsRemaining = static_cast<int>(random(2, 5));
      save->roster.push_back(cp);
    }

    long long totalWage = 0;
    for (const auto& p : save->roster) totalWage += p.wage;
    save->wageBudget = totalWage * 130 / 100;
    save->transferBudget = 15000000;
  }

  CreatePage(e_PageID_CareerHub);
}

// ---------------------------------------------------------------------------
// CareerHubPage
// ---------------------------------------------------------------------------

CareerHubPage::CareerHubPage(Gui2WindowManager* windowManager, const Gui2PageData& pageData)
    : Gui2Page(windowManager, pageData) {
  AddCareerBackdrop(this, windowManager, "career_hub_backdrop");

  Gui2Frame* summaryPanel =
      AddCareerPanel(this, windowManager, "career_hub_summary_panel", 5, 6, 90, 20);
  Gui2Frame* sportingPanel =
      AddCareerPanel(this, windowManager, "career_hub_sporting_panel", 5, 30, 42, 52);
  Gui2Frame* operationsPanel =
      AddCareerPanel(this, windowManager, "career_hub_operations_panel", 53, 30, 42, 52);

  const Vector3 accentColor =
      windowManager->GetStyle()->GetColor(e_DecorationType_Bright2);
  AddCareerCaption(summaryPanel, windowManager, "caption_careerhub_tag", 4, 8, 20, 3,
                   "Career Hub", &accentColor);
  AddCareerCaption(summaryPanel, windowManager, "caption_careerhub_title", 4, 16, 28, 4,
                   "Club Control");

  Gui2Button* btnTransfers =
      new Gui2Button(windowManager, "btn_transfers", 0, 0, 30, 4.5f, "Transfer Market");
  Gui2Button* btnFreeAgency =
      new Gui2Button(windowManager, "btn_freeagency", 0, 0, 30, 4.5f, "Free Agency");
  Gui2Button* btnSquad = new Gui2Button(windowManager, "btn_squad", 0, 0, 30, 4.5f, "Squad Planner");
  Gui2Button* btnTraining = new Gui2Button(windowManager, "btn_training", 0, 0, 30, 4.5f, "Training");
  Gui2Button* btnStrategy =
      new Gui2Button(windowManager, "btn_strategy", 0, 0, 30, 4.5f, "Strategy & Tactics");
  Gui2Button* btnYouth = new Gui2Button(windowManager, "btn_youth", 0, 0, 30, 4.5f, "Youth Academy");
  Gui2Button* btnPressConf =
      new Gui2Button(windowManager, "btn_pressconf", 0, 0, 30, 4.5f, "Press Conference");
  Gui2Button* btnLeagueExp =
      new Gui2Button(windowManager, "btn_leagueexp", 0, 0, 30, 4.5f, "League Expansion");
  Gui2Button* btnCustomLeague =
      new Gui2Button(windowManager, "btn_customleague", 0, 0, 30, 4.5f, "Custom League");
  Gui2Button* btnSeason =
      new Gui2Button(windowManager, "btn_season_end", 26, 86, 48, 5.0f, "Open End of Season");

  btnTransfers->sig_OnClick.connect([this](...) { GoTransferMarket(); });
  btnFreeAgency->sig_OnClick.connect([this](...) { GoFreeAgency(); });
  btnSquad->sig_OnClick.connect([this](...) { GoSquad(); });
  btnTraining->sig_OnClick.connect([this](...) { GoTraining(); });
  btnStrategy->sig_OnClick.connect([this](...) { GoStrategy(); });
  btnYouth->sig_OnClick.connect([this](...) { GoYouthAcademy(); });
  btnPressConf->sig_OnClick.connect([this](...) { GoPressConference(); });
  btnLeagueExp->sig_OnClick.connect([this](...) { GoLeagueExpansion(); });
  btnCustomLeague->sig_OnClick.connect([this](...) { GoCustomLeague(); });
  btnSeason->sig_OnClick.connect([this](...) { GoSeason(); });

  CareerSave* activeSave = CareerDatabase::GetInstance().GetActiveSave();
  if (activeSave) {
    const std::string modeDisplay = GetCareerModeTitle(activeSave->mode);

    AddCareerCaption(summaryPanel, windowManager, "caption_hub_team", 4, 24, 44, 2.8f,
                     modeDisplay + " | " + activeSave->name + " | " + activeSave->leagueName);

    AddCareerCaption(summaryPanel, windowManager, "caption_hub_fin", 4, 36, 22, 2.4f,
                     "Transfer Budget");
    AddCareerCaption(summaryPanel, windowManager, "caption_hub_fin_value", 4, 41, 22, 3,
                     FormatMoney(activeSave->transferBudget));
    AddCareerCaption(summaryPanel, windowManager, "caption_hub_wage", 28, 36, 18, 2.4f,
                     "Wage Budget");
    AddCareerCaption(summaryPanel, windowManager, "caption_hub_wage_value", 28, 41, 18, 3,
                     FormatMoney(activeSave->wageBudget));
    
    AddCareerCaption(summaryPanel, windowManager, "caption_hub_board", 52, 36, 16, 2.4f,
                     "Board Confidence");
    AddCareerCaption(summaryPanel, windowManager, "caption_hub_board_value", 52, 41, 16, 3,
                     std::to_string(activeSave->boardConfidence) + "%");
    AddCareerCaption(summaryPanel, windowManager, "caption_hub_rep", 72, 36, 14, 2.4f,
                     "Reputation");
    AddCareerCaption(summaryPanel, windowManager, "caption_hub_rep_value", 72, 41, 14, 3,
                     CareerDatabase::GetInstance().GetReputationStatus());

    AddCareerCaption(summaryPanel, windowManager, "caption_hub_season", 52, 12, 20, 2.4f,
                     "Season " + std::to_string(activeSave->seasonsPlayed + 1),
                     &kCareerMutedText);
    AddCareerCaption(summaryPanel, windowManager, "caption_hub_squad", 72, 12, 14, 2.4f,
                     std::to_string(activeSave->roster.size()) + " Players",
                     &kCareerMutedText);
    AddCareerCaption(summaryPanel, windowManager, "caption_hub_training", 52, 18, 20, 2.4f,
                     std::to_string(activeSave->trainingPoints) + " Training Pts",
                     &kCareerMutedText);
    AddCareerCaption(summaryPanel, windowManager, "caption_hub_youth", 72, 18, 14, 2.4f,
                     std::to_string(activeSave->youthAcademy.size()) + " Youth",
                     &kCareerMutedText);
  }

  AddCareerCaption(sportingPanel, windowManager, "caption_hub_sporting", 6, 8, 20, 3,
                   "Sporting", &accentColor);
  AddCareerCaption(sportingPanel, windowManager, "caption_hub_sporting_sub", 6, 14, 24, 2.5f,
                   "Prepare the squad each week.", &kCareerMutedText);

  Gui2Grid* sportingGrid = new Gui2Grid(windowManager, "hub_sporting_grid", 5, 22, 31, 44);
  sportingGrid->AddView(btnSquad, 0, 0);
  sportingGrid->AddView(btnStrategy, 1, 0);
  sportingGrid->AddView(btnTraining, 2, 0);
  sportingGrid->AddView(btnYouth, 3, 0);
  sportingGrid->UpdateLayout(0.8f, 0.8f, 1.0f, 1.0f);
  sportingPanel->AddView(sportingGrid);
  sportingGrid->Show();

  AddCareerCaption(operationsPanel, windowManager, "caption_hub_operations", 6, 8, 24, 3,
                   "Front Office", &accentColor);
  AddCareerCaption(operationsPanel, windowManager, "caption_hub_operations_sub", 6, 14, 24, 2.5f,
                   "Transfers, media, and league shape.", &kCareerMutedText);

  Gui2Grid* operationsGrid = new Gui2Grid(windowManager, "hub_operations_grid", 5, 22, 31, 44);
  operationsGrid->AddView(btnTransfers, 0, 0);
  operationsGrid->AddView(btnFreeAgency, 1, 0);
  operationsGrid->AddView(btnPressConf, 2, 0);
  operationsGrid->AddView(btnLeagueExp, 3, 0);
  operationsGrid->AddView(btnCustomLeague, 4, 0);
  operationsGrid->UpdateLayout(0.8f, 0.8f, 1.0f, 1.0f);
  operationsPanel->AddView(operationsGrid);
  operationsGrid->Show();

  this->AddView(btnSeason);
  btnSeason->Show();

  btnSquad->SetFocus();
  this->Show();
}

CareerHubPage::~CareerHubPage() {}

void CareerHubPage::GoTransferMarket() {
  CreatePage(e_PageID_CareerTransferMarket);
}
void CareerHubPage::GoSquad() {
  CreatePage(e_PageID_CareerSquadRoster);
}
void CareerHubPage::GoPressConference() {
  CreatePage(e_PageID_CareerPressConference);
}
void CareerHubPage::GoLeagueExpansion() {
  CreatePage(e_PageID_CareerLeagueExpansion);
}
void CareerHubPage::GoCustomLeague() {
  CreatePage(e_PageID_CareerCustomLeague);
}
void CareerHubPage::GoFreeAgency() {
  CreatePage(e_PageID_CareerFreeAgency);
}
void CareerHubPage::GoTraining() {
  CreatePage(e_PageID_CareerTraining);
}
void CareerHubPage::GoStrategy() {
  CreatePage(e_PageID_CareerStrategy);
}
void CareerHubPage::GoYouthAcademy() {
  CreatePage(e_PageID_CareerYouthAcademy);
}
void CareerHubPage::GoSeason() {
  CreatePage(e_PageID_CareerSeason);
}

// ---------------------------------------------------------------------------
// CareerTransferMarketPage
// ---------------------------------------------------------------------------

CareerTransferMarketPage::CareerTransferMarketPage(Gui2WindowManager* windowManager,
                                                   const Gui2PageData& pageData)
    : Gui2Page(windowManager, pageData) {
  AddCareerBackdrop(this, windowManager, "career_transfer_market_backdrop");
  CareerDatabase::GetInstance().PopulateTransferMarket();

  CareerSave* save = CareerDatabase::GetInstance().GetActiveSave();
  std::string budgetStr = save ? ("Transfer Budget: €" + std::to_string(save->transferBudget)) : "No active save";

  Gui2Caption* title =
      new Gui2Caption(windowManager, "caption_tm_title", 10, 3, 80, 3, "Transfer Market");
  this->AddView(title);
  title->Show();

  Gui2Caption* budget =
      new Gui2Caption(windowManager, "caption_tm_budget", 10, 7, 80, 2, budgetStr);
  this->AddView(budget);
  budget->Show();

  Gui2Caption* header =
      new Gui2Caption(windowManager, "caption_tm_header", 3, 10, 94, 2,
                      "Name                  | POS | OVR | POT | Age | Value          | Asking Price");
  this->AddView(header);
  header->Show();

  auto targets = CareerDatabase::GetInstance().GetTransferTargets();
  Gui2Grid* grid = new Gui2Grid(windowManager, "grid_tm", 3, 13, 94, 60);
  int row = 0;
  for (const auto& t : targets) {
    if (row >= 16) break;
    char buf[256];
    snprintf(buf, sizeof(buf), "%-20s | %-3s | %2d  | %2d  | %2d  | %12lld  | %12lld",
             t.name.c_str(), t.preferredPosition.c_str(),
             t.overallRating, t.potentialRating, t.age,
             t.value, t.askingPrice);
    Gui2Button* btn = new Gui2Button(windowManager, "btn_tm_" + std::to_string(row), 0, 0, 90, 2.5, buf);
    btn->sig_OnClick.connect([this, t](...) {
      Properties props;
      props.Set("playerName", t.name);
      props.Set("askingPrice", std::to_string(t.askingPrice));
      props.Set("playerWage", std::to_string(t.wage));
      CreatePage(e_PageID_CareerTransferBidDetail, props);
    });
    grid->AddView(btn, row++, 0);
  }
  grid->UpdateLayout(0.5);
  this->AddView(grid);
  grid->Show();

  Gui2Button* btnBids = new Gui2Button(windowManager, "btn_tm_mybids", 5, 80, 40, 3, "My Bids");
  btnBids->sig_OnClick.connect([this](...) { CreatePage(e_PageID_CareerTransferBids); });
  this->AddView(btnBids);
  btnBids->Show();

  Gui2Button* btnProcess = new Gui2Button(windowManager, "btn_tm_process", 50, 80, 40, 3, "Process Pending Bids");
  btnProcess->sig_OnClick.connect([this](...) {
    CareerDatabase::GetInstance().ProcessPendingBids();
    CreatePage(e_PageID_CareerTransferBids);
  });
  this->AddView(btnProcess);
  btnProcess->Show();

  Gui2Button* btnBack = new Gui2Button(windowManager, "btn_tm_back", 30, 90, 40, 3, "Back to Hub");
  btnBack->sig_OnClick.connect([this](...) { CreatePage(e_PageID_CareerHub); });
  this->AddView(btnBack);
  btnBack->Show();
  btnBack->SetFocus();

  this->Show();
}

CareerTransferMarketPage::~CareerTransferMarketPage() {}

// ---------------------------------------------------------------------------
// CareerTransferBidsPage
// ---------------------------------------------------------------------------

CareerTransferBidsPage::CareerTransferBidsPage(Gui2WindowManager* windowManager,
                                               const Gui2PageData& pageData)
    : Gui2Page(windowManager, pageData) {
  AddCareerBackdrop(this, windowManager, "career_transfer_bids_backdrop");
  Gui2Caption* title =
      new Gui2Caption(windowManager, "caption_bids_title", 10, 5, 80, 3, "My Transfer Bids");
  this->AddView(title);
  title->Show();

  auto& bids = CareerDatabase::GetInstance().GetActiveBids();
  if (bids.empty()) {
    Gui2Caption* info =
        new Gui2Caption(windowManager, "caption_bids_empty", 10, 20, 80, 4, "No active bids.");
    this->AddView(info);
    info->Show();
  } else {
    Gui2Caption* header =
        new Gui2Caption(windowManager, "caption_bids_header", 5, 12, 90, 2,
                        "Player                 | Bid Amount      | Wage    | Yrs | Status");
    this->AddView(header);
    header->Show();

    Gui2Grid* grid = new Gui2Grid(windowManager, "grid_bids", 5, 15, 90, 60);
    int row = 0;
    for (const auto& b : bids) {
      char buf[256];
      snprintf(buf, sizeof(buf), "%-22s | €%-14lld | €%-6d | %d   | %s",
               b.playerName.c_str(), b.bidAmount, b.offeredWage, b.contractYears,
               CareerDatabase::GetInstance().GetBidStatusString(b.status).c_str());
      Gui2Button* btn = new Gui2Button(windowManager, "btn_bid_" + std::to_string(row), 0, 0, 86, 2.5, buf);
      if (b.status == BidStatus::ACCEPTED) {
        std::string pName = b.playerName;
        btn->sig_OnClick.connect([this, pName](...) {
          CareerDatabase::GetInstance().CompleteTransfer(pName);
          CreatePage(e_PageID_CareerTransferBids);
        });
      } else if (b.status == BidStatus::PENDING) {
        std::string pName = b.playerName;
        btn->sig_OnClick.connect([this, pName](...) { NegotiateBid(pName); });
      }
      grid->AddView(btn, row++, 0);
    }
    grid->UpdateLayout(0.5);
    this->AddView(grid);
    grid->Show();
  }

  Gui2Button* btnBack = new Gui2Button(windowManager, "btn_bids_back", 5, 85, 40, 3, "Back to Market");
  btnBack->sig_OnClick.connect([this](...) { CreatePage(e_PageID_CareerTransferMarket); });
  this->AddView(btnBack);
  btnBack->Show();

  Gui2Button* btnHub = new Gui2Button(windowManager, "btn_bids_hub", 50, 85, 40, 3, "Back to Hub");
  btnHub->sig_OnClick.connect([this](...) { CreatePage(e_PageID_CareerHub); });
  this->AddView(btnHub);
  btnHub->Show();
  btnBack->SetFocus();

  this->Show();
}

CareerTransferBidsPage::~CareerTransferBidsPage() {}

void CareerTransferBidsPage::NegotiateBid(const std::string& playerName) {
  auto& bids = CareerDatabase::GetInstance().GetActiveBids();
  for (auto& b : bids) {
    if (b.playerName == playerName && b.status == BidStatus::PENDING) {
      long long increase = b.bidAmount / 10;
      if (increase < 50000) increase = 50000;
      b.bidAmount += increase;
      b.agentFee = b.bidAmount / 20;
      b.negotiationRounds++;
      break;
    }
  }
  CreatePage(e_PageID_CareerTransferBids);
}

// ---------------------------------------------------------------------------
// CareerTransferBidDetailPage
// ---------------------------------------------------------------------------

CareerTransferBidDetailPage::CareerTransferBidDetailPage(Gui2WindowManager* windowManager,
                                                         const Gui2PageData& pageData)
    : Gui2Page(windowManager, pageData) {
  AddCareerBackdrop(this, windowManager, "career_transfer_detail_backdrop");
  m_playerName = pageData.properties ? pageData.properties->Get("playerName", "") : "";
  m_askingPrice = pageData.properties ? atoll(pageData.properties->Get("askingPrice", "0").c_str()) : 0;
  m_playerWage = pageData.properties ? atoll(pageData.properties->Get("playerWage", "0").c_str()) : 0;

  auto targets = CareerDatabase::GetInstance().GetTransferTargets();
  TransferTarget target;
  bool found = false;
  for (const auto& t : targets) {
    if (t.name == m_playerName) { target = t; found = true; break; }
  }

  Gui2Caption* title =
      new Gui2Caption(windowManager, "caption_detail_title", 10, 5, 80, 3, "Transfer: " + m_playerName);
  this->AddView(title);
  title->Show();

  if (found) {
    std::string info1 = "Position: " + target.preferredPosition + " | OVR: " + std::to_string(target.overallRating) +
                       " | POT: " + std::to_string(target.potentialRating) + " | Age: " + std::to_string(target.age);
    Gui2Caption* line1 = new Gui2Caption(windowManager, "caption_detail_info1", 10, 12, 80, 3, info1);
    this->AddView(line1);
    line1->Show();

    std::string info2 = "Value: €" + std::to_string(target.value) +
                       " | Asking Price: €" + std::to_string(target.askingPrice) +
                       " | Wage: €" + std::to_string(target.wage);
    Gui2Caption* line2 = new Gui2Caption(windowManager, "caption_detail_info2", 10, 16, 80, 3, info2);
    this->AddView(line2);
    line2->Show();

    Gui2Grid* grid = new Gui2Grid(windowManager, "grid_detail", 10, 25, 80, 40);

    long long askPrice = target.askingPrice;
    Gui2Button* bidFull = new Gui2Button(windowManager, "btn_bid_full", 0, 0, 76, 3,
      "Bid Asking Price: €" + std::to_string(askPrice));
    bidFull->sig_OnClick.connect([this, askPrice](...) { PlaceBidForPlayer(askPrice); });
    grid->AddView(bidFull, 0, 0);

    long long bid80 = target.askingPrice * 80 / 100;
    Gui2Button* bid80Button = new Gui2Button(windowManager, "btn_bid_80", 0, 0, 76, 3,
      "Bid 80%: €" + std::to_string(bid80) + " (may be rejected)");
    bid80Button->sig_OnClick.connect([this, bid80](...) { PlaceBidForPlayer(bid80); });
    grid->AddView(bid80Button, 1, 0);

    long long bid60 = target.askingPrice * 60 / 100;
    Gui2Button* bid60Button = new Gui2Button(windowManager, "btn_bid_60", 0, 0, 76, 3,
      "Bid 60%: €" + std::to_string(bid60) + " (likely rejected)");
    bid60Button->sig_OnClick.connect([this, bid60](...) { PlaceBidForPlayer(bid60); });
    grid->AddView(bid60Button, 2, 0);

    grid->UpdateLayout(0.5);
    this->AddView(grid);
    grid->Show();

    CareerSave* save = CareerDatabase::GetInstance().GetActiveSave();
    if (save) {
      long long totalWithFee = target.askingPrice + (target.askingPrice / 20);
      std::string feeNote = "Agent fee (5%): €" + std::to_string(target.askingPrice / 20) +
                           " | Total cost: €" + std::to_string(totalWithFee);
      if (totalWithFee > save->transferBudget) {
        feeNote += " | WARNING: Exceeds budget!";
      }
      Gui2Caption* fee = new Gui2Caption(windowManager, "caption_detail_fee", 10, 68, 80, 4, feeNote);
      this->AddView(fee);
      fee->Show();
    }
  }

  Gui2Button* btnBack = new Gui2Button(windowManager, "btn_detail_back", 30, 85, 40, 3, "Back to Market");
  btnBack->sig_OnClick.connect([this](...) { CreatePage(e_PageID_CareerTransferMarket); });
  this->AddView(btnBack);
  btnBack->Show();
  btnBack->SetFocus();

  this->Show();
}

CareerTransferBidDetailPage::~CareerTransferBidDetailPage() {}

void CareerTransferBidDetailPage::PlaceBidForPlayer(long long amount) {
  TransferBid bid = CareerDatabase::GetInstance().PlaceBid(m_playerName, amount, static_cast<int>(m_playerWage), 3);
  if (bid.status == BidStatus::REJECTED) {
    Gui2Caption* warn =
        new Gui2Caption(windowManager, "caption_bid_warn", 10, 78, 80, 3, "Bid rejected - insufficient budget!");
    this->AddView(warn);
    warn->Show();
  } else {
    CreatePage(e_PageID_CareerTransferBids);
  }
}

// ---------------------------------------------------------------------------
// CareerPressConferencePage  (6.13)
// ---------------------------------------------------------------------------

CareerPressConferencePage::CareerPressConferencePage(Gui2WindowManager* windowManager,
                                                     const Gui2PageData& pageData)
    : Gui2Page(windowManager, pageData) {
  AddCareerBackdrop(this, windowManager, "career_press_backdrop");
  Gui2Caption* title =
      new Gui2Caption(windowManager, "caption_pressconf", 10, 5, 80, 3, "Press Conference");
  this->AddView(title);
  title->Show();

  Gui2Caption* question =
      new Gui2Caption(windowManager, "caption_pc_question", 10, 12, 80, 5,
                      "How do you feel about the team's performance this week?");
  this->AddView(question);
  question->Show();

  // Answer buttons: positive / neutral / negative
  Gui2Button* btnPositive =
      new Gui2Button(windowManager, "btn_pc_positive", 0, 0, 76, 3,
                     "We gave everything – the fans should be proud. (+reputation)");
  Gui2Button* btnNeutral = new Gui2Button(windowManager, "btn_pc_neutral", 0, 0, 76, 3,
                                          "It was a decent performance; we move on.");
  Gui2Button* btnNegative = new Gui2Button(windowManager, "btn_pc_negative", 0, 0, 76, 3,
                                           "I'm disappointed. We must do better. (-reputation)");

  btnPositive->sig_OnClick.connect([this](...) { SelectAnswer(0); });
  btnNeutral->sig_OnClick.connect([this](...) { SelectAnswer(1); });
  btnNegative->sig_OnClick.connect([this](...) { SelectAnswer(2); });

  Gui2Grid* grid = new Gui2Grid(windowManager, "pc_grid", 12, 22, 76, 50);
  grid->AddView(btnPositive, 0, 0);
  grid->AddView(btnNeutral, 1, 0);
  grid->AddView(btnNegative, 2, 0);
  grid->UpdateLayout(0.5);

  this->AddView(grid);
  grid->Show();

  btnPositive->SetFocus();
  this->Show();
}

CareerPressConferencePage::~CareerPressConferencePage() {}

void CareerPressConferencePage::SelectAnswer(int answerIndex) {
  int delta = m_reputationDeltas[answerIndex];
  CareerDatabase::GetInstance().AddEvent("press_conference",
    delta > 0 ? "Positive press conference (+reputation)" :
    delta < 0 ? "Negative press conference (-reputation)" :
    "Neutral press conference",
    delta, false);
  if (delta > 0) {
    CareerDatabase::GetInstance().ModifyBoardConfidence(1);
  } else if (delta < 0) {
    CareerDatabase::GetInstance().ModifyBoardConfidence(-2);
  }
  CreatePage(e_PageID_CareerHub);
}

// ---------------------------------------------------------------------------
// CareerLeagueExpansionPage  (6.16)
// ---------------------------------------------------------------------------

CareerLeagueExpansionPage::CareerLeagueExpansionPage(Gui2WindowManager* windowManager,
                                                     const Gui2PageData& pageData)
    : Gui2Page(windowManager, pageData) {
  AddCareerBackdrop(this, windowManager, "career_expansion_backdrop");
  Gui2Caption* title = new Gui2Caption(windowManager, "caption_leagueexp", 10, 5, 80, 3,
                                       "League Expansion & Relegation");
  this->AddView(title);
  title->Show();

  Gui2Caption* info = new Gui2Caption(windowManager, "caption_leagueexp_info", 10, 12, 80, 3,
                                      "Configure promotion and relegation for your league.");
  this->AddView(info);
  info->Show();

  Gui2Button* btnEnable = new Gui2Button(windowManager, "btn_leagueexp_enable", 0, 0, 60, 3,
                                         "Enable Promotion / Relegation");
  Gui2Button* btnDisable = new Gui2Button(windowManager, "btn_leagueexp_disable", 0, 0, 60, 3,
                                          "Disable Promotion / Relegation");
  Gui2Button* btnAddDiv =
      new Gui2Button(windowManager, "btn_leagueexp_adddiv", 0, 0, 60, 3, "Add Division");

  btnEnable->sig_OnClick.connect([this](...) { EnableRelegation(); });
  btnDisable->sig_OnClick.connect([this](...) { DisableRelegation(); });
  btnAddDiv->sig_OnClick.connect([this](...) { AddDivision(); });

  Gui2Grid* grid = new Gui2Grid(windowManager, "leagueexp_grid", 20, 20, 60, 60);
  grid->AddView(btnEnable, 0, 0);
  grid->AddView(btnDisable, 1, 0);
  grid->AddView(btnAddDiv, 2, 0);
  grid->UpdateLayout(0.5);

  this->AddView(grid);
  grid->Show();

  btnEnable->SetFocus();
  this->Show();
}

CareerLeagueExpansionPage::~CareerLeagueExpansionPage() {}

void CareerLeagueExpansionPage::EnableRelegation() {
  // Future: persist setting via CareerDatabase
  CreatePage(e_PageID_CareerHub);
}

void CareerLeagueExpansionPage::DisableRelegation() {
  // Future: persist setting via CareerDatabase
  CreatePage(e_PageID_CareerHub);
}

void CareerLeagueExpansionPage::AddDivision() {
  // Future: open a sub-page to configure the new division
  CreatePage(e_PageID_CareerHub);
}

// ---------------------------------------------------------------------------
// CareerCustomLeaguePage  (6.17)
// ---------------------------------------------------------------------------

CareerCustomLeaguePage::CareerCustomLeaguePage(Gui2WindowManager* windowManager,
                                               const Gui2PageData& pageData)
    : Gui2Page(windowManager, pageData) {
  AddCareerBackdrop(this, windowManager, "career_custom_backdrop");
  Gui2Caption* title = new Gui2Caption(windowManager, "caption_customleague", 10, 5, 80, 3,
                                       "Custom League Creation");
  this->AddView(title);
  title->Show();

  Gui2Caption* info =
      new Gui2Caption(windowManager, "caption_customleague_info", 10, 12, 80, 3,
                      "Design your own league: name, divisions, teams and cup competition.");
  this->AddView(info);
  info->Show();

  Gui2Button* btnCreate = new Gui2Button(windowManager, "btn_customleague_create", 30, 50, 40, 3,
                                         "Create Custom League");
  btnCreate->sig_OnClick.connect([this](...) { CreateCustomLeague(); });
  this->AddView(btnCreate);
  btnCreate->Show();
  btnCreate->SetFocus();

  this->Show();
}

CareerCustomLeaguePage::~CareerCustomLeaguePage() {}

void CareerCustomLeaguePage::CreateCustomLeague() {
  // Future: collect name/divisions from user input widgets and persist via CareerDatabase
  CreatePage(e_PageID_CareerHub);
}

// ---------------------------------------------------------------------------
// CareerFreeAgencyPage
// ---------------------------------------------------------------------------

CareerFreeAgencyPage::CareerFreeAgencyPage(Gui2WindowManager* windowManager,
                                           const Gui2PageData& pageData)
    : Gui2Page(windowManager, pageData) {
  AddCareerBackdrop(this, windowManager, "career_free_agency_backdrop");
  Gui2Caption* title =
      new Gui2Caption(windowManager, "caption_freeagency", 10, 5, 80, 3, "Free Agency / Recruiting");
  this->AddView(title);
  title->Show();

  CareerSave* activeSave = CareerDatabase::GetInstance().GetActiveSave();
  if (activeSave) {
    Gui2Grid* grid = new Gui2Grid(windowManager, "fa_grid", 10, 15, 80, 70);
    int row = 0;
    for (const CareerPlayer& fa : activeSave->freeAgents) {
      std::string label = fa.name + " | OVR: " + std::to_string(fa.overallRating) + 
                          " | Wage: €" + std::to_string(fa.wage);
      Gui2Button* btn = new Gui2Button(windowManager, "btn_recruit_" + fa.name, 0, 0, 76, 3, "Recruit " + label);
      btn->sig_OnClick.connect([this, fa](...) { RecruitPlayer(fa.name); });
      grid->AddView(btn, row++, 0);
    }
    grid->UpdateLayout(0.5);
    this->AddView(grid);
    grid->Show();
  }

  Gui2Button* btnBack = new Gui2Button(windowManager, "btn_fa_back", 30, 90, 40, 3, "Back to Hub");
  btnBack->sig_OnClick.connect([this](...) { CreatePage(e_PageID_CareerHub); });
  this->AddView(btnBack);
  btnBack->Show();
  btnBack->SetFocus();

  this->Show();
}

CareerFreeAgencyPage::~CareerFreeAgencyPage() {}

void CareerFreeAgencyPage::RecruitPlayer(const std::string& playerName) {
  CareerDatabase::GetInstance().RecruitFreeAgent(playerName);
  CreatePage(e_PageID_CareerFreeAgency);
}

// ---------------------------------------------------------------------------
// CareerTrainingPage
// ---------------------------------------------------------------------------

CareerTrainingPage::CareerTrainingPage(Gui2WindowManager* windowManager,
                                       const Gui2PageData& pageData)
    : Gui2Page(windowManager, pageData) {
  AddCareerBackdrop(this, windowManager, "career_training_backdrop");
  Gui2Caption* title =
      new Gui2Caption(windowManager, "caption_training", 10, 5, 80, 3, "Squad Training");
  this->AddView(title);
  title->Show();

  CareerSave* activeSave = CareerDatabase::GetInstance().GetActiveSave();
  int tp = activeSave ? activeSave->trainingPoints : 0;
  
  Gui2Caption* info =
      new Gui2Caption(windowManager, "caption_tp", 10, 15, 80, 3, "Available Training Points: " + std::to_string(tp));
  this->AddView(info);
  info->Show();

  Gui2Grid* grid = new Gui2Grid(windowManager, "train_grid", 15, 28, 70, 52);

  Gui2Button* btnGeneral = new Gui2Button(windowManager, "btn_train_gen", 0, 0, 66, 3, "General Training (+Form All, -1 TP)");
  btnGeneral->sig_OnClick.connect([this](...) { TrainSquad(); });
  grid->AddView(btnGeneral, 0, 0);

  Gui2Button* btnAttacking = new Gui2Button(windowManager, "btn_train_atk", 0, 0, 66, 3, "Attacking Focus (CF/LM/RM/AM +OVR, -1 TP)");
  btnAttacking->sig_OnClick.connect([this](...) { TrainFocus("Attacking"); });
  grid->AddView(btnAttacking, 1, 0);

  Gui2Button* btnDefending = new Gui2Button(windowManager, "btn_train_def", 0, 0, 66, 3, "Defending Focus (CB/LB/RB/DM/GK +OVR, -1 TP)");
  btnDefending->sig_OnClick.connect([this](...) { TrainFocus("Defending"); });
  grid->AddView(btnDefending, 2, 0);

  Gui2Button* btnPhysical = new Gui2Button(windowManager, "btn_train_phy", 0, 0, 66, 3, "Physical Focus (+Morale +Form All, -1 TP)");
  btnPhysical->sig_OnClick.connect([this](...) { TrainFocus("Physical"); });
  grid->AddView(btnPhysical, 3, 0);

  Gui2Button* btnTactical = new Gui2Button(windowManager, "btn_train_tac", 0, 0, 66, 3, "Tactical Focus (CM/DM/AM +OVR, -1 TP)");
  btnTactical->sig_OnClick.connect([this](...) { TrainFocus("Tactical"); });
  grid->AddView(btnTactical, 4, 0);

  Gui2Button* btnShooting = new Gui2Button(windowManager, "btn_train_shoot", 0, 0, 66, 3, "Shooting Focus (CF/LM/RM +OVR, -1 TP)");
  btnShooting->sig_OnClick.connect([this](...) { TrainFocus("Shooting"); });
  grid->AddView(btnShooting, 5, 0);

  grid->UpdateLayout(0.5);
  this->AddView(grid);
  grid->Show();

  Gui2Button* btnBack = new Gui2Button(windowManager, "btn_tr_back", 30, 90, 40, 3, "Back to Hub");
  btnBack->sig_OnClick.connect([this](...) { CreatePage(e_PageID_CareerHub); });
  this->AddView(btnBack);
  btnBack->Show();
  btnGeneral->SetFocus();

  this->Show();
}

CareerTrainingPage::~CareerTrainingPage() {}

void CareerTrainingPage::TrainSquad() {
  if (CareerDatabase::GetInstance().TrainSquad()) {
    CreatePage(e_PageID_CareerTraining);
  }
}

void CareerTrainingPage::TrainFocus(const std::string& focusArea) {
  if (CareerDatabase::GetInstance().TrainFocus(focusArea)) {
    CreatePage(e_PageID_CareerTraining);
  }
}

// ---------------------------------------------------------------------------
// CareerStrategyPage
// ---------------------------------------------------------------------------

CareerStrategyPage::CareerStrategyPage(Gui2WindowManager* windowManager,
                                       const Gui2PageData& pageData)
    : Gui2Page(windowManager, pageData) {
  AddCareerBackdrop(this, windowManager, "career_strategy_backdrop");
  Gui2Caption* title =
      new Gui2Caption(windowManager, "caption_strategy", 10, 5, 80, 3, "Team Strategy");
  this->AddView(title);
  title->Show();

  CareerSave* activeSave = CareerDatabase::GetInstance().GetActiveSave();
  std::string curStrat = activeSave ? activeSave->activeStrategy : "None";
  
  Gui2Caption* info =
      new Gui2Caption(windowManager, "caption_curstrat", 10, 15, 80, 3, "Current Strategy: " + curStrat);
  this->AddView(info);
  info->Show();

  Gui2Grid* grid = new Gui2Grid(windowManager, "strat_grid", 20, 30, 60, 40);
  
  Gui2Button* btnAttacking = new Gui2Button(windowManager, "btn_strat_atk", 0, 0, 60, 3, "Attacking");
  btnAttacking->sig_OnClick.connect([this](...) { SetStrategy("Attacking"); });
  grid->AddView(btnAttacking, 0, 0);

  Gui2Button* btnBalanced = new Gui2Button(windowManager, "btn_strat_bal", 0, 0, 60, 3, "Balanced");
  btnBalanced->sig_OnClick.connect([this](...) { SetStrategy("Balanced"); });
  grid->AddView(btnBalanced, 1, 0);

  Gui2Button* btnDefensive = new Gui2Button(windowManager, "btn_strat_def", 0, 0, 60, 3, "Defensive");
  btnDefensive->sig_OnClick.connect([this](...) { SetStrategy("Defensive"); });
  grid->AddView(btnDefensive, 2, 0);

  grid->UpdateLayout(0.5);
  this->AddView(grid);
  grid->Show();

  Gui2Button* btnBack = new Gui2Button(windowManager, "btn_st_back", 30, 90, 40, 3, "Back to Hub");
  btnBack->sig_OnClick.connect([this](...) { CreatePage(e_PageID_CareerHub); });
  this->AddView(btnBack);
  btnBack->Show();
  btnAttacking->SetFocus();

  this->Show();
}

CareerStrategyPage::~CareerStrategyPage() {}

void CareerStrategyPage::SetStrategy(const std::string& strategyName) {
  CareerDatabase::GetInstance().SetStrategy(strategyName);
  CreatePage(e_PageID_CareerStrategy); // reload
}

// ---------------------------------------------------------------------------
// CareerYouthAcademyPage
// ---------------------------------------------------------------------------

CareerYouthAcademyPage::CareerYouthAcademyPage(Gui2WindowManager* windowManager,
                                               const Gui2PageData& pageData)
    : Gui2Page(windowManager, pageData) {
  AddCareerBackdrop(this, windowManager, "career_youth_backdrop");
  Gui2Caption* title =
      new Gui2Caption(windowManager, "caption_youth", 10, 5, 80, 3, "Youth Academy");
  this->AddView(title);
  title->Show();

  CareerSave* activeSave = CareerDatabase::GetInstance().GetActiveSave();
  if (activeSave) {
    Gui2Grid* grid = new Gui2Grid(windowManager, "ya_grid", 10, 15, 80, 60);
    int row = 0;
    
    // Scout button
    int scoutCost = 50000 * activeSave->scoutingNetworkLevel;
    Gui2Button* btnScout = new Gui2Button(windowManager, "btn_scout_youth", 0, 0, 76, 3, 
                                          "Scout New Talent (-€" + std::to_string(scoutCost) + ")");
    btnScout->sig_OnClick.connect([this](...) { ScoutPlayer(); });
    grid->AddView(btnScout, row++, 0);

    for (const CareerPlayer& ya : activeSave->youthAcademy) {
      std::string label = ya.name + " | Age: " + std::to_string(ya.age) + 
                          " | OVR: " + std::to_string(ya.overallRating) + 
                          " | POT: " + std::to_string(ya.potentialRating);
      Gui2Button* btn = new Gui2Button(windowManager, "btn_promote_" + ya.name, 0, 0, 76, 3, "Promote " + label);
      btn->sig_OnClick.connect([this, ya](...) { PromotePlayer(ya.name); });
      grid->AddView(btn, row++, 0);
    }
    grid->UpdateLayout(0.5);
    this->AddView(grid);
    grid->Show();
    btnScout->SetFocus();
  }

  Gui2Button* btnBack = new Gui2Button(windowManager, "btn_ya_back", 30, 90, 40, 3, "Back to Hub");
  btnBack->sig_OnClick.connect([this](...) { CreatePage(e_PageID_CareerHub); });
  this->AddView(btnBack);
  btnBack->Show();

  this->Show();
}

CareerYouthAcademyPage::~CareerYouthAcademyPage() {}

void CareerYouthAcademyPage::ScoutPlayer() {
  CareerDatabase::GetInstance().ScoutYouthPlayer();
  CreatePage(e_PageID_CareerYouthAcademy); // refresh
}

void CareerYouthAcademyPage::PromotePlayer(const std::string& playerName) {
  CareerDatabase::GetInstance().PromoteYouthPlayer(playerName);
  CreatePage(e_PageID_CareerYouthAcademy);
}

// ---------------------------------------------------------------------------
// CareerSquadRosterPage
// ---------------------------------------------------------------------------

CareerSquadRosterPage::CareerSquadRosterPage(Gui2WindowManager* windowManager,
                                             const Gui2PageData& pageData)
    : Gui2Page(windowManager, pageData) {
  AddCareerBackdrop(this, windowManager, "career_squad_backdrop");
  Gui2Caption* title =
      new Gui2Caption(windowManager, "caption_squad", 10, 3, 80, 3, "My Squad");
  this->AddView(title);
  title->Show();

  CareerSave* activeSave = CareerDatabase::GetInstance().GetActiveSave();
  if (activeSave) {
    long long totalWage = 0;
    for (const auto& p : activeSave->roster) {
      totalWage += p.wage;
    }
    Gui2Caption* header =
      new Gui2Caption(windowManager, "caption_squad_header", 5, 6, 90, 2,
        "Name              | POS | OVR | POT | Age | Value          | Wage       | Morale | Form | Contract");
    this->AddView(header);
    header->Show();

    Gui2Grid* grid = new Gui2Grid(windowManager, "squad_grid", 5, 9, 90, 72);
    int row = 0;
    for (const auto& player : activeSave->roster) {
      std::string formStr = CareerDatabase::GetInstance().GetFormString(player.matchForm);
      std::string moraleStr = CareerDatabase::GetInstance().GetMoraleString(player.morale);

      char labelBuf[256];
      snprintf(labelBuf, sizeof(labelBuf),
        "%-17s | %-3s | %2d  | %2d  | %2d  | %10lld  | %9lld | %-7s | %-5s | %d yr",
        player.name.c_str(), player.preferredPosition.c_str(),
        player.overallRating, player.potentialRating, player.age,
        player.value, player.wage,
        moraleStr.c_str(), formStr.c_str(), player.contractYearsRemaining);

      std::string label(labelBuf);
      Gui2Button* btn = new Gui2Button(windowManager, "btn_release_" + std::to_string(row), 0, 0, 84, 2.5, label);
      btn->sig_OnClick.connect([this, player](...) { ReleasePlayer(player.name); });
      grid->AddView(btn, row++, 0);
    }
    grid->UpdateLayout(0.5);
    this->AddView(grid);
    grid->Show();

    Gui2Caption* footer =
      new Gui2Caption(windowManager, "caption_squad_footer", 5, 83, 90, 2,
        "Total Squad: " + std::to_string(activeSave->roster.size()) + " players | Total Wage Bill: €" + std::to_string(totalWage) + "/week");
    this->AddView(footer);
    footer->Show();
  }

  Gui2Button* btnBack = new Gui2Button(windowManager, "btn_squad_back", 30, 90, 40, 3, "Back to Hub");
  btnBack->sig_OnClick.connect([this](...) { CreatePage(e_PageID_CareerHub); });
  this->AddView(btnBack);
  btnBack->Show();

  this->Show();
}

CareerSquadRosterPage::~CareerSquadRosterPage() {}

void CareerSquadRosterPage::ReleasePlayer(const std::string& playerName) {
  CareerDatabase::GetInstance().ReleasePlayer(playerName);
  CreatePage(e_PageID_CareerSquadRoster);
}

// ---------------------------------------------------------------------------
// CareerSeasonPage
// ---------------------------------------------------------------------------

CareerSeasonPage::CareerSeasonPage(Gui2WindowManager* windowManager,
                                   const Gui2PageData& pageData)
    : Gui2Page(windowManager, pageData) {
  AddCareerBackdrop(this, windowManager, "career_season_backdrop");
  Gui2Caption* title =
      new Gui2Caption(windowManager, "caption_season", 10, 5, 80, 3, "End of Season");
  this->AddView(title);
  title->Show();

  CareerSave* activeSave = CareerDatabase::GetInstance().GetActiveSave();
  if (activeSave) {
    Gui2Caption* info =
      new Gui2Caption(windowManager, "caption_season_info", 10, 12, 80, 3,
        "Current Season: " + std::to_string(activeSave->seasonsPlayed + 1) +
        " | Board Confidence: " + std::to_string(activeSave->boardConfidence) + "%" +
        " | Reputation: " + CareerDatabase::GetInstance().GetReputationStatus());
    this->AddView(info);
    info->Show();

    Gui2Caption* warning =
      new Gui2Caption(windowManager, "caption_season_warn", 10, 18, 80, 4,
        "WARNING: Advancing the season will process player growth, contract expiries, "
        "budget renewal, and age all players. This cannot be undone.");
    this->AddView(warning);
    warning->Show();

    if (!activeSave->seasonSummaries.empty()) {
      Gui2Caption* histTitle =
        new Gui2Caption(windowManager, "caption_season_hist", 10, 28, 80, 2, "Past Seasons:");
      this->AddView(histTitle);
      histTitle->Show();

      Gui2Grid* histGrid = new Gui2Grid(windowManager, "season_hist_grid", 10, 30, 80, 30);
      int row = 0;
      int startIdx = std::max(0, static_cast<int>(activeSave->seasonSummaries.size()) - 5);
      for (int i = startIdx; i < static_cast<int>(activeSave->seasonSummaries.size()); i++) {
        Gui2Caption* entry =
          new Gui2Caption(windowManager, "caption_hist_" + std::to_string(row), 0, 0, 76, 2,
                          activeSave->seasonSummaries[i]);
        histGrid->AddView(entry, row++, 0);
      }
      histGrid->UpdateLayout(0.5);
      this->AddView(histGrid);
      histGrid->Show();
    }
  }

  Gui2Button* btnAdvance = new Gui2Button(windowManager, "btn_season_advance", 25, 70, 50, 4, ">> ADVANCE SEASON >>");
  btnAdvance->sig_OnClick.connect([this](...) { AdvanceSeason(); });
  this->AddView(btnAdvance);
  btnAdvance->Show();
  btnAdvance->SetFocus();

  Gui2Button* btnBack = new Gui2Button(windowManager, "btn_season_back", 30, 90, 40, 3, "Back to Hub");
  btnBack->sig_OnClick.connect([this](...) { GoToHub(); });
  this->AddView(btnBack);
  btnBack->Show();

  this->Show();
}

CareerSeasonPage::~CareerSeasonPage() {}

void CareerSeasonPage::AdvanceSeason() {
  CareerDatabase::GetInstance().AdvanceSeason();
  CreatePage(e_PageID_CareerHub);
}

void CareerSeasonPage::GoToHub() {
  CreatePage(e_PageID_CareerHub);
}
