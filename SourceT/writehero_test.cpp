#include <gtest/gtest.h>
#include "picosha2.h"
#include <fstream>
#include <vector>
#include <cstdio>
#include "all.h"
#include "paths.h"

using namespace devilution;

int spelldat_vanilla[] = {
	0, 1, 1, 4, 5, -1, 3, 3, 6, -1, 7, 6, 8, 9,
	8, 9, -1, -1, -1, -1, 3, 11, -1, 14, -1, -1,
	-1, -1, -1, 8, 1, 1, -1, 2, 1, 14, 9
};

static void PackItemUnique(PkItemStruct *id, int idx)
{
	id->idx = idx;
	id->iCreateInfo = 0x2DE;
	id->bId = 1 + 2 * ITEM_QUALITY_UNIQUE;
	id->bDur = 40;
	id->bMDur = 40;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x1C0C44B0;
}

static void PackItemStaff(PkItemStruct *id)
{
	id->idx = 150;
	id->iCreateInfo = 0x2010;
	id->bId = 1 + 2 * ITEM_QUALITY_MAGIC;
	id->bDur = 75;
	id->bMDur = 75;
	id->bCh = 12;
	id->bMCh = 12;
	id->iSeed = 0x2A15243F;
}

static void PackItemBow(PkItemStruct *id)
{
	id->idx = 145;
	id->iCreateInfo = 0x0814;
	id->bId = 1 + 2 * ITEM_QUALITY_MAGIC;
	id->bDur = 60;
	id->bMDur = 60;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x449D8992;
}

static void PackItemSword(PkItemStruct *id)
{
	id->idx = 122;
	id->iCreateInfo = 0x081E;
	id->bId = 1 + 2 * ITEM_QUALITY_MAGIC;
	id->bDur = 60;
	id->bMDur = 60;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x680FAC02;
}

static void PackItemRing1(PkItemStruct *id)
{
	id->idx = 153;
	id->iCreateInfo = 0xDE;
	id->bId = 1 + 2 * ITEM_QUALITY_MAGIC;
	id->bDur = 0;
	id->bMDur = 0;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x5B41AFA8;
}

static void PackItemRing2(PkItemStruct *id)
{
	id->idx = 153;
	id->iCreateInfo = 0xDE;
	id->bId = 1 + 2 * ITEM_QUALITY_MAGIC;
	id->bDur = 0;
	id->bMDur = 0;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x1E41FEFC;
}

static void PackItemAmulet(PkItemStruct *id)
{
	id->idx = 155;
	id->iCreateInfo = 0xDE;
	id->bId = 1 + 2 * ITEM_QUALITY_MAGIC;
	id->bDur = 0;
	id->bMDur = 0;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x70A0383A;
}

static void PackItemArmor(PkItemStruct *id)
{
	id->idx = 70;
	id->iCreateInfo = 0xDE;
	id->bId = 1 + 2 * ITEM_QUALITY_MAGIC;
	id->bDur = 90;
	id->bMDur = 90;
	id->bCh = 0;
	id->bMCh = 0;
	id->iSeed = 0x63AAC49B;
}

static void PackItemFullRejuv(PkItemStruct *id, int i)
{
	const Uint32 seeds[] = { 0x7C253335, 0x3EEFBFF8, 0x76AFB1A9, 0x38EB45FE, 0x1154E197, 0x5964B644, 0x76B58BEB, 0x002A6E5A };
	id->idx = ItemMiscIdIdx(IMISC_FULLREJUV);
	id->iSeed = seeds[i];
	id->iCreateInfo = 0;
	id->bId = 2 * ITEM_QUALITY_NORMAL;
	id->bDur = 0;
	id->bMDur = 0;
	id->bCh = 0;
	id->bMCh = 0;
}

static int PrepareInvSlot(PkPlayerStruct *pPack, int pos, int size, int start = 0)
{
	static char ret = 0;
	if (start)
		ret = 0;
	++ret;
	if (size == 0) {
		pPack->InvGrid[pos] = ret;
	} else if (size == 1) {
		pPack->InvGrid[pos] = ret;
		pPack->InvGrid[pos - 10] = -ret;
		pPack->InvGrid[pos - 20] = -ret;
	} else if (size == 2) {
		pPack->InvGrid[pos] = ret;
		pPack->InvGrid[pos + 1] = -ret;
		pPack->InvGrid[pos - 10] = -ret;
		pPack->InvGrid[pos - 10 + 1] = -ret;
		pPack->InvGrid[pos - 20] = -ret;
		pPack->InvGrid[pos - 20 + 1] = -ret;
	} else if (size == 3) {
		pPack->InvGrid[pos] = ret;
		pPack->InvGrid[pos + 1] = -ret;
		pPack->InvGrid[pos - 10] = -ret;
		pPack->InvGrid[pos - 10 + 1] = -ret;
	} else {
		abort();
	}
	return ret - 1;
}

static void PackPlayerTest(PkPlayerStruct *pPack)
{
	memset(pPack, 0, 0x4F2);
	pPack->destAction = -1;
	pPack->destParam1 = 0;
	pPack->destParam2 = 0;
	pPack->plrlevel = 0;
	pPack->pExperience = 1583495809;
	pPack->pLevel = 50;
	pPack->px = 75;
	pPack->py = 68;
	pPack->targx = 75;
	pPack->targy = 68;
	pPack->pGold = 0;
	pPack->pStatPts = 0;
	pPack->pDiabloKillLevel = 3;
	for (auto i = 0; i < 40; i++)
		pPack->InvList[i].idx = -1;
	for (auto i = 0; i < 7; i++)
		pPack->InvBody[i].idx = -1;
	for (auto i = 0; i < MAXBELTITEMS; i++)
		PackItemFullRejuv(pPack->SpdList + i, i);
	for (auto i = 1; i < 37; i++) {
		if (spelldat_vanilla[i] != -1) {
			pPack->pMemSpells |= 1ULL << (i - 1);
			pPack->pSplLvl[i] = 15;
		}
	}
	for (auto i = 0; i < 7; i++)
		pPack->InvBody[i].idx = -1;
	strcpy(pPack->pName, "TestPlayer");
	pPack->pClass = PC_ROGUE;
	pPack->pBaseStr = 20 + 35;
	pPack->pBaseMag = 15 + 55;
	pPack->pBaseDex = 30 + 220;
	pPack->pBaseVit = 20 + 60;
	pPack->pHPBase = ((20 + 10) << 6) + ((20 + 10) << 5) + 48 * 128 + (60 << 6);
	pPack->pMaxHPBase = pPack->pHPBase;
	pPack->pManaBase = (15 << 6) + (15 << 5) + 48 * 128 + (55 << 6);
	pPack->pMaxManaBase = pPack->pManaBase;

	PackItemUnique(pPack->InvBody + INVLOC_HEAD, 52);
	PackItemRing1(pPack->InvBody + INVLOC_RING_LEFT);
	PackItemRing2(pPack->InvBody + INVLOC_RING_RIGHT);
	PackItemAmulet(pPack->InvBody + INVLOC_AMULET);
	PackItemArmor(pPack->InvBody + INVLOC_CHEST);
	PackItemBow(pPack->InvBody + INVLOC_HAND_LEFT);

	PackItemStaff(pPack->InvList + PrepareInvSlot(pPack, 28, 2, 1));
	PackItemSword(pPack->InvList + PrepareInvSlot(pPack, 20, 1));

	pPack->_pNumInv = 2;
}

static int CountItems(ItemStruct *items, int n)
{
	int count = n;
	for (int i = 0; i < n; i++)
		if (items[i].isEmpty())
			count--;

	return count;
}

static int Count8(Sint8 *ints, int n)
{
	int count = n;
	for (int i = 0; i < n; i++)
		if (ints[i] == 0)
			count--;

	return count;
}

static int CountBool(bool *bools, int n)
{
	int count = n;
	for (int i = 0; i < n; i++)
		if (!bools[i])
			count--;

	return count;
}

static void AssertPlayer(PlayerStruct *pPlayer)
{
	ASSERT_EQ(Count8(pPlayer->_pSplLvl, 64), 23);
	ASSERT_EQ(Count8(pPlayer->InvGrid, NUM_INV_GRID_ELEM), 9);
	ASSERT_EQ(CountItems(pPlayer->InvBody, NUM_INVLOC), 6);
	ASSERT_EQ(CountItems(pPlayer->InvList, NUM_INV_GRID_ELEM), 2);
	ASSERT_EQ(CountItems(pPlayer->SpdList, MAXBELTITEMS), 8);
	ASSERT_EQ(CountItems(&pPlayer->HoldItem, 1), 1);

	ASSERT_EQ(pPlayer->_px, 75);
	ASSERT_EQ(pPlayer->_py, 68);
	ASSERT_EQ(pPlayer->_pfutx, 75);
	ASSERT_EQ(pPlayer->_pfuty, 68);
	ASSERT_EQ(pPlayer->_ptargx, 75);
	ASSERT_EQ(pPlayer->_ptargy, 68);
	ASSERT_EQ(pPlayer->plrlevel, 0);
	ASSERT_EQ(pPlayer->destAction, -1);
	ASSERT_STREQ(pPlayer->_pName, "TestPlayer");
	ASSERT_EQ(pPlayer->_pClass, 1);
	ASSERT_EQ(pPlayer->_pBaseStr, 55);
	ASSERT_EQ(pPlayer->_pStrength, 124);
	ASSERT_EQ(pPlayer->_pBaseMag, 70);
	ASSERT_EQ(pPlayer->_pMagic, 80);
	ASSERT_EQ(pPlayer->_pBaseDex, 250);
	ASSERT_EQ(pPlayer->_pDexterity, 281);
	ASSERT_EQ(pPlayer->_pBaseVit, 80);
	ASSERT_EQ(pPlayer->_pVitality, 90);
	ASSERT_EQ(pPlayer->_pLevel, 50);
	ASSERT_EQ(pPlayer->_pStatPts, 0);
	ASSERT_EQ(pPlayer->_pExperience, 1583495809);
	ASSERT_EQ(pPlayer->_pGold, 0);
	ASSERT_EQ(pPlayer->_pMaxHPBase, 12864);
	ASSERT_EQ(pPlayer->_pHPBase, 12864);
	ASSERT_EQ(pPlayer->_pBaseToBlk, 20);
	ASSERT_EQ(pPlayer->_pMaxManaBase, 11104);
	ASSERT_EQ(pPlayer->_pManaBase, 11104);
	ASSERT_EQ(pPlayer->_pMemSpells, 66309357295);
	ASSERT_EQ(pPlayer->_pNumInv, 2);
	ASSERT_EQ(pPlayer->wReflections, 0);
	ASSERT_EQ(pPlayer->pTownWarps, 0);
	ASSERT_EQ(pPlayer->pDungMsgs, 0);
	ASSERT_EQ(pPlayer->pDungMsgs2, 0);
	ASSERT_EQ(pPlayer->pLvlLoad, 0);
	ASSERT_EQ(pPlayer->pDiabloKillLevel, 3);
	ASSERT_EQ(pPlayer->pBattleNet, 0);
	ASSERT_EQ(pPlayer->pManaShield, 0);
	ASSERT_EQ(pPlayer->pDifficulty, 0);
	ASSERT_EQ(pPlayer->pDamAcFlags, 0);

	ASSERT_EQ(pPlayer->_pmode, 0);
	ASSERT_EQ(Count8(pPlayer->walkpath, MAX_PATH_LENGTH), 25);
	ASSERT_EQ(pPlayer->_pgfxnum, 36);
	ASSERT_EQ(pPlayer->_pAnimDelay, 3);
	ASSERT_EQ(pPlayer->_pAnimCnt, 1);
	ASSERT_EQ(pPlayer->_pAnimLen, 20);
	ASSERT_EQ(pPlayer->_pAnimFrame, 1);
	ASSERT_EQ(pPlayer->_pAnimWidth, 96);
	ASSERT_EQ(pPlayer->_pAnimWidth2, 16);
	ASSERT_EQ(pPlayer->_pSpell, -1);
	ASSERT_EQ(pPlayer->_pSplType, 4);
	ASSERT_EQ(pPlayer->_pSplFrom, 0);
	ASSERT_EQ(pPlayer->_pTSpell, 0);
	ASSERT_EQ(pPlayer->_pTSplType, 0);
	ASSERT_EQ(pPlayer->_pRSpell, -1);
	ASSERT_EQ(pPlayer->_pRSplType, 4);
	ASSERT_EQ(pPlayer->_pSBkSpell, -1);
	ASSERT_EQ(pPlayer->_pSBkSplType, 0);
	ASSERT_EQ(pPlayer->_pAblSpells, 134217728);
	ASSERT_EQ(pPlayer->_pScrlSpells, 0);
	ASSERT_EQ(pPlayer->_pSpellFlags, 0);
	ASSERT_EQ(pPlayer->_pwtype, 1);
	ASSERT_EQ(pPlayer->_pBlockFlag, 0);
	ASSERT_EQ(pPlayer->_pLightRad, 11);
	ASSERT_EQ(pPlayer->_pDamageMod, 101);
	ASSERT_EQ(pPlayer->_pHitPoints, 16640);
	ASSERT_EQ(pPlayer->_pMaxHP, 16640);
	ASSERT_EQ(pPlayer->_pMana, 14624);
	ASSERT_EQ(pPlayer->_pMaxMana, 14624);
	ASSERT_EQ(pPlayer->_pNextExper, 2000);
	ASSERT_EQ(pPlayer->_pMagResist, 75);
	ASSERT_EQ(pPlayer->_pFireResist, 16);
	ASSERT_EQ(pPlayer->_pLghtResist, 75);
	ASSERT_EQ(CountBool(pPlayer->_pLvlVisited, NUMLEVELS), 0);
	ASSERT_EQ(CountBool(pPlayer->_pSLvlVisited, NUMLEVELS), 0);
	ASSERT_EQ(pPlayer->_pNFrames, 20);
	ASSERT_EQ(pPlayer->_pNWidth, 96);
	ASSERT_EQ(pPlayer->_pWFrames, 8);
	ASSERT_EQ(pPlayer->_pWWidth, 96);
	ASSERT_EQ(pPlayer->_pAFrames, 0);
	ASSERT_EQ(pPlayer->_pAWidth, 128);
	ASSERT_EQ(pPlayer->_pAFNum, 0);
	ASSERT_EQ(pPlayer->_pSFrames, 16);
	ASSERT_EQ(pPlayer->_pSWidth, 96);
	ASSERT_EQ(pPlayer->_pSFNum, 12);
	ASSERT_EQ(pPlayer->_pHFrames, 0);
	ASSERT_EQ(pPlayer->_pHWidth, 96);
	ASSERT_EQ(pPlayer->_pDFrames, 20);
	ASSERT_EQ(pPlayer->_pDWidth, 128);
	ASSERT_EQ(pPlayer->_pBFrames, 0);
	ASSERT_EQ(pPlayer->_pBWidth, 96);
	ASSERT_EQ(pPlayer->_pIMinDam, 1);
	ASSERT_EQ(pPlayer->_pIMaxDam, 14);
	ASSERT_EQ(pPlayer->_pIAC, 115);
	ASSERT_EQ(pPlayer->_pIBonusDam, 0);
	ASSERT_EQ(pPlayer->_pIBonusToHit, 0);
	ASSERT_EQ(pPlayer->_pIBonusAC, 0);
	ASSERT_EQ(pPlayer->_pIBonusDamMod, 0);
	ASSERT_EQ(pPlayer->_pISpells, 0);
	ASSERT_EQ(pPlayer->_pIFlags, 0);
	ASSERT_EQ(pPlayer->_pIGetHit, 0);
	ASSERT_EQ(pPlayer->_pISplLvlAdd, 0);
	ASSERT_EQ(pPlayer->_pISplDur, 0);
	ASSERT_EQ(pPlayer->_pIEnAc, 0);
	ASSERT_EQ(pPlayer->_pIFMinDam, 0);
	ASSERT_EQ(pPlayer->_pIFMaxDam, 0);
	ASSERT_EQ(pPlayer->_pILMinDam, 0);
	ASSERT_EQ(pPlayer->_pILMaxDam, 0);
	ASSERT_EQ(pPlayer->pOriginalCathedral, 0);
}

TEST(Writehero, pfile_write_hero)
{
	SetPrefPath(".");
	std::remove("multi_0.sv");

	gbVanilla = true;
	gbIsHellfire = false;
	gbIsMultiplayer = true;
	gbIsHellfireSaveGame = false;
	leveltype = DTYPE_TOWN;

	myplr = 0;
	_uiheroinfo info {};
	strcpy(info.name, "TestPlayer");
	info.heroclass = PC_ROGUE;
	pfile_ui_save_create(&info);
	PkPlayerStruct pks;
	PackPlayerTest(&pks);
	UnPackPlayer(&pks, myplr, TRUE);
	AssertPlayer(&plr[0]);
	pfile_write_hero();

	std::ifstream f("multi_0.sv", std::ios::binary);
	std::vector<unsigned char> s(picosha2::k_digest_size);
	picosha2::hash256(f, s.begin(), s.end());
	EXPECT_EQ(picosha2::bytes_to_hex_string(s.begin(), s.end()),
	    "08e9807d1281e4273268f4e265757b4429cfec7c3e8b6deb89dfa109d6797b1c");
}
