/* Campaigns, the Secret Missions and pilot transfer (campaign.c). */
#ifndef SDL_PORT_CAMPAIGN_H
#define SDL_PORT_CAMPAIGN_H

#ifdef __cplusplus
extern "C" {
#endif

int SdlPlayingSm2(void);
const char *SdlSaveGameFileName(void);
void SdlSm2ClearDisguise(void);
int SdlSm2StartDisguise(short navPoint, short linkedNav, short type);
int SdlSm2Disguised(void);
int SdlSm2Overlooks(short scanner, short other);
void SdlSm2DisguiseHit(short shooter, short victim);
struct PilotRecord;
void SdlSetSm2Pilots(struct PilotRecord *pilots, int fresh);
void SdlSetCampaignShips(void);
extern int nPilotTransferState;
extern int nPilotTransferCampaign;
extern int nLaunchCampaign;
/* SM1.5, the fan port of Super Wing Commander's Covert Missions, launches
 * as SM2 with four files of its own (SdlSelectSm2Files). */
extern int bLaunchSm15;
int SdlTransferPilot(short slot, short campaign);
struct CampaignState;
int SdlValidateExpansionSave(const struct CampaignState *campaign);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_CAMPAIGN_H */
