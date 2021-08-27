#include "luaSub.hpp"
#include "GDPlay.hpp"
#include "GPlayers.h"

//#include "c99_snprintf.h"

//���������[�N���o�p
#include <crtdbg.h>  
#ifdef _DEBUG 
#ifdef __MEMLEAKCHECK 
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__) 
#define malloc(p1) _malloc_dbg((p1),_NORMAL_BLOCK,__FILE__,__LINE__) 
#define __MEMLEAKCHECK
#endif
#endif 
//--���������[�N���o�p

extern CD3DMesh*	m_pLandMesh;	// XMesh�f�[�^


extern CMyD3DApplication* g_pApp;
extern GDPlay *DPlay;
extern GPLAYERDATA PlayerData[];
extern GMYDATA MyPlayerData;
extern GFloat GSPEEDLIMIT;
extern GFloat GFARMAX;
extern GFloat GMARKERSIZE;
extern GFloat GNAMESIZE;

extern lua_State *ScriptL;
extern lua_State *SystemL;

extern int ViewUpdate;
extern GFloat Zoom;

GFloat luaL3dx,luaL3dy,luaL3dz;
GFloat luaL2dx,luaL2dy;
int luaGraColor;
extern int randTime;
extern DWORD frameGetTime;
extern DWORD frameElapsedTime;
extern TCHAR HostName[];
extern DWORD PortNo;
extern char LastChatData[];
void Line(GVector &p1,GVector &p2,unsigned int col);
void Line2D(GFloat x0,GFloat y0,GFloat x1,GFloat y1,int col);
DWORD SetFogColor(DWORD R8G8B8);
DWORD GetFogColor();
D3DXVECTOR3 GetLightColor();


int __luaPrintSub(lua_State *L, int s, int e, char* dest, int destSize) //�X�^�b�N��s�Ԗڂ���e�Ԗڂ܂ł̒l�𕶎��񉻂���dest�֏�������  ��������0,�o�b�t�@�I�[�o�[��������1��Ԃ�  
{
	if (e<s) return 1;
	for (int i = s; i <= e; i++){
		if (destSize<=0) return 1;
		int len = 0;
		if (lua_isnumber(L, i)&&lua_type(L,i)!=LUA_TSTRING){
			len = snprintf(dest, destSize, "%.2f ", lua_tonumber(L, i));
			if(lua_type(L, i+1) != LUA_TNUMBER) len--; //���l�^���A�������Ƃ��̂ݽ�߰��}��
		}else if(lua_isstring(L, i)){
			len = snprintf(dest, destSize, "%s", lua_tostring(L, i));
		}else if(lua_isnil(L, i)){
			len = snprintf(dest, destSize, "%s", "nil");
		}else if(lua_isboolean(L, i)){
			len = snprintf(dest, destSize, "%s", lua_toboolean(L, i) ? "true" : "false");
		}else{
			int __nameType = luaL_getmetafield(L, i, "__name"); //�l��__name�t�B�[���h��������Ȃ炻��𖼑O�ɁA�����łȂ��Ȃ�^�������Ɏg��
			if(__nameType) __nameType = lua_type(L, -1); //5.0�����luaL_getmetafield�͌^��Ԃ����������s��Ԃ����ߎ�蒼��	 �����Ŏ擾�ł���^�͕K��nil�ȊO�ɂȂ�
			const char* pName = (__nameType==LUA_TSTRING)?lua_tostring(L, -1):lua_typename(L, lua_type(L, i));
			len = snprintf(dest, destSize, "%s:%p", pName, lua_topointer(L, i));
			if(__nameType!=LUA_TNIL) lua_pop(L, 1); //nil�łȂ����A__name�t�B�[���h�̒l���ς܂�Ă�̂ŕЕt��
		}

		if (len < 0){ //�{����snprintf�p  ���ϸې��Usnprintf�ł���Ζ��Ӗ�����ۯ�
			*dest = '\0';
			continue;
		}
		destSize -= len;
		dest += len;
	}
	return 0;
}
int luaErrMsgHandler(lua_State* L) //msgh(errMsg) return errMsg
{
	luaL_traceback(L,L,lua_tostring(L,-1),1);
	return 1;
}
int luaGetSMouseX(lua_State *L)
{
	static int m = 0;
	if (CtrlKey == 0 && World->Stop == 0) lua_pushnumber(L, m);
	else {
		lua_pushnumber(L, MouseX);
		m = MouseX;
	}
	return 1;
}
int luaGetSMouseY(lua_State *L)
{
	static int m=0;
	if(CtrlKey==0 && World->Stop==0) lua_pushnumber(L, m );
	else {
		lua_pushnumber(L, MouseY );
		m=MouseY;
	}
    return 1;
}
int luaGetSMouseL(lua_State *L)
{
	if(CtrlKey==0 && World->Stop==0) lua_pushnumber(L, 0 );
	else lua_pushnumber(L, MouseL );
    return 1;
}
int luaGetSMouseR(lua_State *L)
{
	if(CtrlKey==0 && World->Stop==0) lua_pushnumber(L, 0 );
    else lua_pushnumber(L, MouseR );
    return 1;
}
int luaGetSMouseM(lua_State *L)
{
	if(CtrlKey==0 && World->Stop==0) lua_pushnumber(L, 0 );
    else lua_pushnumber(L, MouseM );
    return 1;
}
int luaGetMouseX(lua_State *L)
{
	static int m=0;
	if(CtrlKey==1) lua_pushnumber(L, m );
	else {
		lua_pushnumber(L, MouseX );
		m=MouseX;
	}
    return 1;
}
int luaGetMouseY(lua_State *L)
{
	static int m=0;
	if(CtrlKey==1) lua_pushnumber(L, m );
	else {
		lua_pushnumber(L, MouseY );
		m=MouseY;
	}
    return 1;
}
int luaGetMouseL(lua_State *L)
{
	if(CtrlKey==1) lua_pushnumber(L, 0 );
    else lua_pushnumber(L, MouseL );
    return 1;
}
int luaGetMouseR(lua_State *L)
{
	if(CtrlKey==1) lua_pushnumber(L, 0 );
	else lua_pushnumber(L, MouseR );
    return 1;
}
int luaGetMouseM(lua_State *L)
{
	if(CtrlKey==1) lua_pushnumber(L, 0 );
	else lua_pushnumber(L, MouseM );
    return 1;
}
int luaGetTickCount(lua_State *L)
{
    lua_pushnumber(L, TickCount );
    return 1;
}
int luaSetTicks(lua_State *L)
{
	TickCount=(int)lua_tonumber(L, 1);
    lua_pushnumber(L, TickCount );
	return 1;
}
int luaSetCCDZoom(lua_State *L)
{
	GFloat v=(GFloat)lua_tonumber(L, 1);
	if(v>=1 && v<=100) CCDZoom=v;
    lua_pushnumber(L, CCDZoom );
	return 1;
}

/* �F̫�ϯđI���𑫂��Ă݂����̂́A������������Ɏ��Ƃ�����GC�Ɏ��Ԏ���Ēx���Ȃ���������̂Ŗv
int luaGetCCD(lua_State *L)
{
	int color=CCDImage[(int)lua_tonumber(L, 2)][(int)lua_tonumber(L, 1)];
	if (lua_type(L, 3)==LUA_TSTRING){
		const char* format = lua_tostring(L,3);
		if(!stricmp(format, "R8G8B8")) color=(color&0x7C00)<<9 | (color&0x03E0)<<6 | (color&0x001F)<<3;
		else if(!stricmp(format, "R5G5B5")) color=color;
		else if(!stricmp(format, "R5G6B5")) color=(color&0xFFE0)<<1 | (color&0x001F);
		else luaL_argerror(L,3,"color format invalid");
	}
	lua_pushnumber(L,  (double)color);
	return 1;
}--*/
int luaGetCCD32(lua_State *L)
{
	lua_pushnumber(L,  (double)(CCDImage[(int)lua_tonumber(L, 1)][(int)lua_tonumber(L, 2)]&0xFFFFFF));
	return 1;
}
int luaGetCCD(lua_State *L)
{
	UINT32 rgb=CCDImage[(int)lua_tonumber(L, 1)][(int)lua_tonumber(L, 2)];
	lua_pushnumber(L,  ((rgb&0xF80000)>>9)|((rgb&0xF800)>>6)|((rgb&0xF8)>>3));
	return 1;
}
int luaGetCCDRed(lua_State *L)
{
	lua_pushnumber(L, ((CCDImage[(int)lua_tonumber(L, 1)][(int)lua_tonumber(L, 2)]&0xFF0000)>>16)/256.0);
	return 1;
}
int luaGetCCDGreen(lua_State *L)
{
	lua_pushnumber(L,((CCDImage[(int)lua_tonumber(L, 1)][(int)lua_tonumber(L, 2)]&0x00FF00)>>8)/256.0);
	return 1;
}
int luaGetCCDBlue(lua_State *L)
{
	lua_pushnumber(L,(CCDImage[(int)lua_tonumber(L, 1)][(int)lua_tonumber(L, 2)]&0x0000FF)/256.0);
	return 1;
}

int luaKey(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GKEYMAX) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,(double)KeyList[(int)no].Pressed);
	return 1;
}
int luaKeyDown(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GKEYMAX) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,(double)KeyList[(int)no].Down);
	return 1;
}
int luaKeyUp(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GKEYMAX) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,(double)KeyList[(int)no].Up);
	return 1;
}
int luaKey2(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GKEYMAX) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,(double)KeyList[(int)no].SPressed);
	return 1;
}
int luaKeyDown2(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GKEYMAX) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,(double)KeyList[(int)no].SDown);
	return 1;
}
int luaKeyUp2(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GKEYMAX) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,(double)KeyList[(int)no].SUp);
	return 1;
}
int luaAnalog(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=6) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Analog[(int)no]);
	return 1;
}
int luaHat(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=1) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Hat[(int)no]);
	return 1;
}

int luaGetType(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Chip[(int)no]->ChipType);
	return 1;
}
int luaGetDir(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Chip[(int)no]->Dir);
	return 1;
}
int luaGetOption(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Chip[(int)no]->Option);
	return 1;
}
int luaGetEffect(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Chip[(int)no]->Effect);
	return 1;
}
int luaGetUserOption(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Chip[(int)no]->UserOption);
	return 1;
}
int luaGetUserEffect(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Chip[(int)no]->UserEffect);
	return 1;
}
int luaGetPower(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Chip[(int)no]->Power);
	return 1;
}
int luaGetAngle(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	if(Chip[(int)no]->LinkInfo) lua_pushnumber(L,Chip[(int)no]->LinkInfo->Angle);
	else lua_pushnumber(L,-1);
	return 1;
}
int luaGetSpring(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	if(Chip[(int)no]->LinkInfo) lua_pushnumber(L,Chip[(int)no]->LinkInfo->SpringK);
	else lua_pushnumber(L,-1);
	return 1;
}
int luaGetDamper(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	if(Chip[(int)no]->LinkInfo) lua_pushnumber(L,Chip[(int)no]->LinkInfo->DamperK);
	else lua_pushnumber(L,-1);
	return 1;
}
int luaGetBrake(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	if(Chip[(int)no]->LinkInfo) lua_pushnumber(L,Chip[(int)no]->LinkInfo->FrictionK);
	else lua_pushnumber(L,-1);
	return 1;
}
int luaGetParent(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,-1);return 1;}
	if(Chip[(int)no]->Parent) lua_pushnumber(L,Chip[(int)no]->Parent->ID);
	else lua_pushnumber(L,-1);
	return 1;
}
int luaGetTop(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,-1);return 1;}
	if(Chip[(int)no]->Top) lua_pushnumber(L,Chip[(int)no]->Top->ID);
	else lua_pushnumber(L,-1); 
	return 1;
}
int luaGetColor(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Chip[(int)no]->Color);
	return 1;
}
int luaGetEnergy(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	double e=Chip[(int)no]->Energy;
	if(e<=0) e=0;
	lua_pushnumber(L,e);
	return 1;
}
int luaGetM(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Chip[(int)no]->M);
	return 1;
}

int luaGetObjVx(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GOBJMAX || World->Object[(int)no]==NULL) lua_pushnumber(L,0);
	else lua_pushnumber(L,World->Object[(int)no]->V.x);
	return 1;
}
int luaGetObjVy(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GOBJMAX || World->Object[(int)no]==NULL) lua_pushnumber(L,0);
	else lua_pushnumber(L,World->Object[(int)no]->V.y);
	return 1;
}
int luaGetObjVz(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GOBJMAX || World->Object[(int)no]==NULL) lua_pushnumber(L,0);
	else lua_pushnumber(L,World->Object[(int)no]->V.z);
	return 1;
}
int luaGetObjWx(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GOBJMAX || World->Object[(int)no]==NULL) lua_pushnumber(L,0);
	else lua_pushnumber(L,World->Object[(int)no]->W.x);
	return 1;
}
int luaGetObjWy(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GOBJMAX || World->Object[(int)no]==NULL) lua_pushnumber(L,0);
	else lua_pushnumber(L,World->Object[(int)no]->W.y);
	return 1;
}
int luaGetObjWz(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GOBJMAX || World->Object[(int)no]==NULL) lua_pushnumber(L,0);
	else lua_pushnumber(L,World->Object[(int)no]->W.z);
	return 1;
}
int luaGetObjM(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GOBJMAX || World->Object[(int)no]==NULL) lua_pushnumber(L,0);
	else lua_pushnumber(L,World->Object[(int)no]->M);
	return 1;
}
int luaGetObjI(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	int x=(int)lua_tonumber(L, 2);
	int y=(int)lua_tonumber(L, 3);
	if(no<0 || no>=GOBJMAX || World->Object[(int)no]==NULL) lua_pushnumber(L,0);
	else {
		if(x<0 || x>=3)  {lua_pushnumber(L,0);return 1;}
		if(y<0 || y>=3)  {lua_pushnumber(L,0);return 1;}
		lua_pushnumber(L,World->Object[(int)no]->I.elem[x][y]);
	}
	return 1;
}
int luaGetI(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	int x=(int)lua_tonumber(L, 2);
	int y=(int)lua_tonumber(L, 3);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	if(x<0 || x>=3)  {lua_pushnumber(L,0);return 1;}
	if(y<0 || y>=3)  {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Chip[(int)no]->I.elem[x][y]);
	return 1;
}
int luaPosx(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Chip[(int)no]->X.x);
	return 1;
}
int luaPosy(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Chip[(int)no]->X.y);
	return 1;
}
int luaPosz(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Chip[(int)no]->X.z);
	return 1;
}
int luaGetY(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,-100000.0f);return 1;}
	double h=World->Land->GetY2(Chip[(int)no]->X.x,Chip[(int)no]->X.y,Chip[(int)no]->X.z);
	if(h<0) lua_pushnumber(L,h);
	else  lua_pushnumber(L,Chip[(int)no]->X.y-h);
	return 1;
}
int luaUnLinkBye(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	if(Chip[(int)no]->Parent==NULL) {lua_pushnumber(L,0);return 1;}
	World->DeleteLink(Chip[(int)no]);
	Chip[(int)no]->ByeFlag=1;
	if(ViewType==5 || ViewType==6) LastBye=(int)no;
	lua_pushnumber(L,1);
	return 1;
}
int luaUnLink(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	if(Chip[(int)no]->Parent==NULL) {lua_pushnumber(L,0);return 1;}
	World->DeleteLink(Chip[(int)no]);
	Chip[(int)no]->ByeFlag=0;
	if(ViewType==5 || ViewType==6) LastBye=(int)no;
	lua_pushnumber(L,1);
	return 1;
}
int luaLx(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	GVector l=Chip[(int)no]->L*Chip[(int)no]->Rt;
	lua_pushnumber(L,l.x);
	return 1;
}
int luaLy(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	GVector l=Chip[(int)no]->L*Chip[(int)no]->Rt;
	lua_pushnumber(L,l.y);
	return 1;
}
int luaLz(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	GVector l=Chip[(int)no]->L*Chip[(int)no]->Rt;
	lua_pushnumber(L,l.z);
	return 1;
}
int luaWx(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	GVector l=Chip[(int)no]->W*Chip[(int)no]->Rt;
	lua_pushnumber(L,l.x);
	return 1;
}
int luaWy(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	GVector l=Chip[(int)no]->W*Chip[(int)no]->Rt;
	lua_pushnumber(L,l.y);
	return 1;
}
int luaWz(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount)  {lua_pushnumber(L,0);return 1;}
	GVector l=Chip[(int)no]->W*Chip[(int)no]->Rt;
	lua_pushnumber(L,l.z);
	return 1;
}
int luaVx(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount)  {lua_pushnumber(L,0);return 1;}
	GVector l=Chip[(int)no]->V*Chip[(int)no]->Rt;
	lua_pushnumber(L,l.x);
	return 1;
}
int luaVy(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	GVector l=Chip[(int)no]->V*Chip[(int)no]->Rt;
	lua_pushnumber(L,l.y);
	return 1;
}
int luaVz(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	GVector l=Chip[(int)no]->V*Chip[(int)no]->Rt;
	lua_pushnumber(L,l.z);
	return 1;
}
int luaFx(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	GVector l=Chip[(int)no]->P*Chip[(int)no]->Rt;
	lua_pushnumber(L,l.x);
	return 1;
}
int luaFy(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	GVector l=Chip[(int)no]->P*Chip[(int)no]->Rt;
	lua_pushnumber(L,l.y);
	return 1;
}
int luaFz(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	GVector l=Chip[(int)no]->P*Chip[(int)no]->Rt;
	lua_pushnumber(L,l.z);
	return 1;
}
int luaAx(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	GVector vx=GVector(1,0,0)*Chip[(int)no]->R;
	GVector vy=GVector(0,1,0);
	GVector vz=GVector(0,0,1)*Chip[(int)no]->R;
	double a=(vy).angle(vz)-M_PI/2.0;
	lua_pushnumber(L,a);
	return 1;
}
int luaAy(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	double a=-(GVector(0,0,1)*Chip[(int)no]->R).Cut2(GVector(0,1,0)).angle2(GVector(0,0,1),GVector(0,1,0));
	lua_pushnumber(L,a);
	return 1;
}
int luaAz(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	GVector vx=GVector(1,0,0)*Chip[(int)no]->R;
	GVector vy=GVector(0,1,0);
	GVector vz=GVector(0,0,1)*Chip[(int)no]->R;
	double a=(vy).angle(vx)-M_PI/2.0;
	lua_pushnumber(L,a);
	return 1;
}
int luaEx(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=ChipCount) {lua_pushnumber(L,0);return 1;}
	GVector  y_axis, z_axis;
	z_axis.x = Chip[n]->R.elem[2][0];//m[2];
	z_axis.y = Chip[n]->R.elem[2][1];//m[5];
	z_axis.z = Chip[n]->R.elem[2][2];//m[8];

	double yaw = atan2( z_axis.x, z_axis.z );

	double  cos_yaw = cos( yaw );
	lua_pushnumber(L,atan2( (double)(cos_yaw * z_axis.y), (double)fabs( z_axis.z ) ));
	return 1;
}
int luaEy(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=ChipCount) {lua_pushnumber(L,0);return 1;}
	GVector  y_axis, z_axis;
	z_axis.x = Chip[n]->R.elem[2][0];//m[2];
	z_axis.z = Chip[n]->R.elem[2][2];//m[8];

	lua_pushnumber(L,atan2( z_axis.x, z_axis.z ));
	return 1;
}
int luaEz(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=ChipCount) {lua_pushnumber(L,0);return 1;}
	GVector  y_axis, z_axis;
	z_axis.x = Chip[n]->R.elem[2][0];//m[2];
	z_axis.y = Chip[n]->R.elem[2][1];//m[5];
	z_axis.z = Chip[n]->R.elem[2][2];//m[8];
	y_axis.x = Chip[n]->R.elem[1][0];//m[1];
	y_axis.y = Chip[n]->R.elem[1][1];//m[4];
	y_axis.z = Chip[n]->R.elem[1][2];//m[7];

	double yaw = atan2( z_axis.x, z_axis.z );

	double  cos_yaw = cos( yaw );
	double pitch=atan2( (double)(cos_yaw * z_axis.y), (double)fabs( z_axis.z ) );

	double  sin_yaw = sin( yaw );
	double  cos_pitch = cos( pitch );
	lua_pushnumber(L,atan2( (double)(cos_pitch * ( sin_yaw * y_axis.z - cos_yaw * y_axis.x )), (double)y_axis.y ));
	return 1;
}

int luaRx(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	int rn=(int)lua_tonumber(L, 2);
	if(n<0 || n>=ChipCount) {lua_pushnumber(L,0);return 1;}
	if(rn<0 || rn>=ChipCount)  {lua_pushnumber(L,0);return 1;}
	GVector  y_axis, z_axis;
	GMatrix R=Chip[n]->R*Chip[rn]->R.transpose();
	z_axis.x = R.elem[2][0];//m[2];
	z_axis.y = R.elem[2][1];//m[5];
	z_axis.z = R.elem[2][2];//m[8];

	double yaw = atan2( z_axis.x, z_axis.z );

	double  cos_yaw = cos( yaw );
	lua_pushnumber(L,atan2( (double)(cos_yaw * z_axis.y), (double)fabs( z_axis.z ) ));
	return 1;
}
int luaRy(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	int rn=(int)lua_tonumber(L, 2);
	if(n<0 || n>=ChipCount) {lua_pushnumber(L,0);return 1;}
	if(rn<0 || rn>=ChipCount)  {lua_pushnumber(L,0);return 1;}
	GVector  y_axis, z_axis;
	GMatrix R=Chip[n]->R*Chip[rn]->R.inverse();
	z_axis.x = R.elem[2][0];//m[2];
	z_axis.z = R.elem[2][2];//m[8];

	lua_pushnumber(L,atan2( z_axis.x, z_axis.z ));
	return 1;
}
int luaRz(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	int rn=(int)lua_tonumber(L, 2);
	if(n<0 || n>=ChipCount) {lua_pushnumber(L,0);return 1;}
	if(rn<0 || rn>=ChipCount)  {lua_pushnumber(L,0);return 1;}
	GMatrix R=Chip[n]->R*Chip[rn]->R.transpose();
	GVector  y_axis, z_axis;
	z_axis.x = R.elem[2][0];//m[2];
	z_axis.y = R.elem[2][1];//m[5];
	z_axis.z = R.elem[2][2];//m[8];
	y_axis.x = R.elem[1][0];//m[1];
	y_axis.y = R.elem[1][1];//m[4];
	y_axis.z = R.elem[1][2];//m[7];

	double yaw = atan2( z_axis.x, z_axis.z );

	double  cos_yaw = cos( yaw );
	double pitch=atan2( (double)(cos_yaw * z_axis.y), (double)fabs( z_axis.z ) );

	double  sin_yaw = sin( yaw );
	double  cos_pitch = cos( pitch );
	lua_pushnumber(L,atan2( (double)(cos_pitch * ( sin_yaw * y_axis.z - cos_yaw * y_axis.x )), (double)y_axis.y ));
	return 1;
}
int luaGx(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->Top->TotalCenterOfGravity.x);
	return 1;
}
int luaGy(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->Top->TotalCenterOfGravity.y);
	return 1;
}
int luaGz(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->Top->TotalCenterOfGravity.z);
	return 1;
}
int luaXx(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->R.elem[0][0]);
	return 1;
}
int luaXy(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->R.elem[0][1]);
	return 1;
}
int luaXz(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->R.elem[0][2]);
	return 1;
}
int luaYx(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->R.elem[1][0]);
	return 1;
}
int luaYy(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->R.elem[1][1]);
	return 1;
}
int luaYz(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->R.elem[1][2]);
	return 1;
}
int luaZx(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->R.elem[2][0]);
	return 1;
}
int luaZy(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->R.elem[2][1]);
	return 1;
}
int luaZz(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->R.elem[2][2]);
	return 1;
}
int luaQx(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->Q.x);
	return 1;
}
int luaQy(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->Q.y);
	return 1;
}
int luaQz(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->Q.z);
	return 1;
}
int luaQw(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->Q.w);
	return 1;
}
int luaObjPosx(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GOBJMAX || World->Object[(int)no]==NULL) lua_pushnumber(L,0);
	else lua_pushnumber(L,World->Object[(int)no]->X.x);
	return 1;
}
int luaObjPosy(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GOBJMAX || World->Object[(int)no]==NULL) lua_pushnumber(L,-100000.0);
	else lua_pushnumber(L,World->Object[(int)no]->X.y);
	return 1;
}
int luaObjPosz(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=GOBJMAX || World->Object[(int)no]==NULL) lua_pushnumber(L,0);
	else lua_pushnumber(L,World->Object[(int)no]->X.z);
	return 1;
}
int luaVel(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,0);return 1;}
	lua_pushnumber(L,Chip[(int)no]->V.abs());
	return 1;
}
int luaChips(lua_State *L)
{
	lua_pushnumber(L,(double)ChipCount);
	return 1;
}
int luaWeight(lua_State *L)
{
	lua_pushnumber(L,(double)Chip[0]->TotalMass);
	return 1;
}
int luaRnd(lua_State *L)
{
	lua_pushnumber(L,(double)rand()/(double)RAND_MAX);
	return 1;
}

int luaGetDt(lua_State *L)
{
	lua_pushnumber(L,(double)World->Dt);
	return 1;
}
int luaGetWidth(lua_State *L)
{
	lua_pushnumber(L,(double)Width);
	return 1;
}
int luaGetHeight(lua_State *L)
{
	lua_pushnumber(L,(double)Height);
	return 1;
}
int luaGetFps(lua_State *L)
{
	int flag=(int)lua_tonumber(L, 1);
	if(flag==1 && frameElapsedTime>=1){
		lua_pushnumber(L,1000/(double)frameElapsedTime);
	}else{
		lua_pushnumber(L,(double)FPS);
	}
	return 1;
}
int luaGetBase(lua_State *L)
{
	lua_pushnumber(L,(double)LIMITFPS);
	return 1;
}
int luaGetFaces(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no==1){
		lua_pushnumber(L,(double)NumVertice);
	}else{
		lua_pushnumber(L,(double)NumFace);
	}
	return 1;
}
int luaGetTolerant(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) lua_pushnumber(L,0);
	else lua_pushnumber(L,Chip[(int)no]->Tolerant);
	return 1;
}
int luaMove3D(lua_State *L)
{
	luaL3dx=(GFloat)lua_tonumber(L, 1);
	luaL3dy=(GFloat)lua_tonumber(L, 2);
	luaL3dz=(GFloat)lua_tonumber(L, 3);
	lua_pushnumber(L,0);
	return 1;
}
int luaLine3D(lua_State *L)
{
	GFloat x=(GFloat)lua_tonumber(L, 1);
	GFloat y=(GFloat)lua_tonumber(L, 2);
	GFloat z=(GFloat)lua_tonumber(L, 3);
	if(ViewUpdate)	{
		g_pApp->ViewSet();
		ViewUpdate=0;
	}
	Line(GVector(luaL3dx,luaL3dy,luaL3dz),GVector(x,y,z),luaGraColor);
	luaL3dx=(GFloat)x;
	luaL3dy=(GFloat)y;
	luaL3dz=(GFloat)z;
	lua_pushnumber(L,0);
	return 1;
}
int luaMove2D(lua_State *L)
{
	luaL2dx=(GFloat)lua_tonumber(L, 1);
	luaL2dy=(GFloat)lua_tonumber(L, 2);
	lua_pushnumber(L,0);
	return 1;
}
int luaLine2D(lua_State *L)
{
	GFloat x=(GFloat)lua_tonumber(L, 1);
	GFloat y=(GFloat)lua_tonumber(L, 2);
	if(ViewUpdate)	{
		g_pApp->ViewSet();
		ViewUpdate=0;
	}
	Line2D(luaL2dx,luaL2dy,x,y,luaGraColor);
	luaL2dx=(GFloat)x;
	luaL2dy=(GFloat)y;
	lua_pushnumber(L,0);

	return 1;
}
int luaSetColor(lua_State *L)
{
	luaGraColor=(int)lua_tonumber(L, 1);
	lua_pushnumber(L,(double)luaGraColor);
	return 1;
}
int luaRound(lua_State *L)
{
	double a=lua_tonumber(L, 1);
	if(a>=0.0)lua_pushnumber(L,(int)(fabs(a)+0.5));
	else lua_pushnumber(L,-(int)(fabs(a)+0.5));
	return 1;
}
int luaLen3(lua_State *L)
{
	double a=lua_tonumber(L, 1);
	double b=lua_tonumber(L, 2);
	double c=lua_tonumber(L, 3);
	lua_pushnumber(L,sqrt(a*a+b*b+c*c));
	return 1;
}
int luaLen2(lua_State *L)
{
	double a=lua_tonumber(L, 1);
	double b=lua_tonumber(L, 2);
	lua_pushnumber(L,sqrt(a*a+b*b));
	return 1;
}
int luaPI(lua_State *L)
{
	lua_pushnumber(L,M_PI);
	return 1;
}
int luaToDeg(lua_State *L)
{
	double a=lua_tonumber(L, 1);
	lua_pushnumber(L,a*180.0/M_PI);
	return 1;
}
int luaToRad(lua_State *L)
{
	double a=lua_tonumber(L, 1);
	lua_pushnumber(L,a*M_PI/180.0);
	return 1;
}

int luaGetPlayers(lua_State *L)
{
	lua_pushnumber(L,(double)DPlay->GetNumPlayers());
	return 1;
}
int luaGetPlayerHostID(lua_State *L)
{
	lua_pushnumber(L,(double)DPlay->GetHostPlayerDPNID());
	return 1;
}
int luaGetPlayerMyID(lua_State *L)
{
	lua_pushnumber(L,(double)DPlay->GetLocalPlayerDPNID());
	return 1;
}
int luaGetPlayerID(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=DPlay->GetNumPlayers()) {
		lua_pushnumber(L,0.0);
		return 1;
	}
	lua_pushnumber(L,(double)PlayerData[n].ReceiveData.info.dpnidPlayer);
	return 1;
}
int luaGetPlayerCrushes(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=DPlay->GetNumPlayers()) {
		lua_pushnumber(L,0.0);
		return 1;
	}
	if(DPlay->GetLocalPlayerDPNID()==PlayerData[n].ReceiveData.info.dpnidPlayer) {
		lua_pushnumber(L,(double)MyPlayerData.crush);
		return 1;
	}
	lua_pushnumber(L,(double)PlayerData[n].crush);
	return 1;
}
int luaGetPlayerResets(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=DPlay->GetNumPlayers()) {
		lua_pushnumber(L,0.0);
		return 1;
	}
	if(DPlay->GetLocalPlayerDPNID()==PlayerData[n].ReceiveData.info.dpnidPlayer) {
		lua_pushnumber(L,(double)MyPlayerData.reset);
		return 1;
	}
	lua_pushnumber(L,(double)PlayerData[n].reset);
	return 1;
}
int luaGetPlayerInits(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=DPlay->GetNumPlayers()) {
		lua_pushnumber(L,0.0);
		return 1;
	}
	if(DPlay->GetLocalPlayerDPNID()==PlayerData[n].ReceiveData.info.dpnidPlayer) {
		lua_pushnumber(L,(double)MyPlayerData.init);
		return 1;
	}
	lua_pushnumber(L,(double)PlayerData[n].init);
	return 1;
}
int luaGetPlayerColor(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=DPlay->GetNumPlayers()) {
		lua_pushnumber(L,0.0);
		return 1;
	}
	lua_pushnumber(L,(double)PlayerData[n].ReceiveData.info.color);
	return 1;
}
int luaGetPlayerChips(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=DPlay->GetNumPlayers()) {
		lua_pushnumber(L,0.0);
		return 1;
	}
	if(DPlay->GetLocalPlayerDPNID()==PlayerData[n].ReceiveData.info.dpnidPlayer) {
		lua_pushnumber(L,(double)ChipCount);
		return 1;
	}
	lua_pushnumber(L,(double)PlayerData[n].ChipCount);
	return 1;
}
int luaGetPlayerX(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=DPlay->GetNumPlayers()) {
		lua_pushnumber(L,0.0);
		return 1;
	}
	double a=pow((double)PlayerData[n].ChipCount,1.0/3.0)/2.0;
	if(DPlay->GetLocalPlayerDPNID()==PlayerData[n].ReceiveData.info.dpnidPlayer) {
		lua_pushnumber(L,(double)Chip[0]->X.x);
		return 1;
	}
	if(!World->haveArm) a=0;
	double v=PlayerData[n].x+(rand()%10000-5000)/1000.0*a+sin(randTime/150.0)*a-sin(randTime/350.0)*a;
	lua_pushnumber(L,v);
	return 1;
}
int luaGetPlayerY(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=DPlay->GetNumPlayers()) {
		lua_pushnumber(L,-100000.0);
		return 1;
	}
	double a=pow((double)PlayerData[n].ChipCount,1.0/3.0)/2.0;
	if(DPlay->GetLocalPlayerDPNID()==PlayerData[n].ReceiveData.info.dpnidPlayer) {
		lua_pushnumber(L,(double)Chip[0]->X.y);
		return 1;
	}
	if (!World->haveArm) a = 0;
	double v=PlayerData[n].y+(rand()%10000-5000)/1000.0*a+sin(randTime/160.0)*a-sin(randTime/360.0)*a;
	lua_pushnumber(L,v);
	return 1;
}
int luaGetPlayerZ(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=DPlay->GetNumPlayers()) {
		lua_pushnumber(L,0.0);
		return 1;
	}
	double a=pow((double)PlayerData[n].ChipCount,1.0/3.0)/2.0;
	if(DPlay->GetLocalPlayerDPNID()==PlayerData[n].ReceiveData.info.dpnidPlayer) {
		lua_pushnumber(L,(double)Chip[0]->X.z);
		return 1;
	}
	if (!World->haveArm) a = 0;
	double v=PlayerData[n].z+(rand()%10000-5000)/1000.0*a+sin(randTime/140.0)*a-sin(randTime/340.0)*a;
	lua_pushnumber(L,v);
	return 1;
}
int luaGetPlayerArms(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=DPlay->GetNumPlayers()) {
		lua_pushnumber(L,0.0);
		return 1;
	}
	if(DPlay->GetLocalPlayerDPNID()==PlayerData[n].ReceiveData.info.dpnidPlayer) {
		lua_pushnumber(L,(double)MyPlayerData.haveArm);
		return 1;
	}
	lua_pushnumber(L,(double)PlayerData[n].haveArm);
	return 1;
}
int luaGetPlayerYForces(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=DPlay->GetNumPlayers()) {
		lua_pushnumber(L,0.0);
		return 1;
	}
	if(DPlay->GetLocalPlayerDPNID()==PlayerData[n].ReceiveData.info.dpnidPlayer) {
		lua_pushnumber(L,(double)MyPlayerData.yforce);
		return 1;
	}
	lua_pushnumber(L,(double)PlayerData[n].yforce);
	return 1;
}
int luaGetPlayerName(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=DPlay->GetNumPlayers()) {
		lua_pushstring(L,"");
		return 1;
	}
	lua_pushstring(L,PlayerData[n].ReceiveData.info.strPlayerName);
	return 1;
}
int luaGetFuel(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,-1);return 1;}
	lua_pushnumber(L,Chip[(int)no]->Top->TotalFuel);
	return 1;
}
int luaGetFuelMax(lua_State *L)
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount) {lua_pushnumber(L,-1);return 1;}
	lua_pushnumber(L,Chip[(int)no]->Top->TotalFuelMax);
	return 1;
}
void luaUpdateVal() {
	int i,k;
	for(i=0;i<VarCount;i++) {
	//for(i=0;i<GVALMAX;i++) {
		for(k=0;k<ValList[i].RefCount;k++) {
			if(ValList[i].Flag[k])
				*(ValList[i].Ref[k])=-ValList[i].Val;
			else *(ValList[i].Ref[k])=ValList[i].Val;
		}
	}
}
//---------------------
int luaGetPlayerPos(lua_State *L)
{
	int n=(int)lua_tonumber(L, 1);
	int chipNo=(int)lua_tonumber(L, 2);
	if(n<0 || n>=DPlay->GetNumPlayers()) {
		lua_pushnumber(L,0.0);
		lua_pushnumber(L,-100000.0f);
		lua_pushnumber(L,0.0);
		return 3;
	}
	if(DPlay->GetLocalPlayerDPNID()==PlayerData[n].ReceiveData.info.dpnidPlayer) {
		if(chipNo<0 || chipNo>=ChipCount) chipNo=0;
		lua_pushnumber(L,(double)Chip[chipNo]->X.x);
		lua_pushnumber(L,(double)Chip[chipNo]->X.y);
		lua_pushnumber(L,(double)Chip[chipNo]->X.z);
		return 3;
	}
	//if(chipNo<0 || chipNo>=PlayerData[n].ChipCount) chipNo=0;
	if(chipNo<0 || chipNo>=PlayerData[n].ChipCount){
		lua_pushnumber(L,0.0);
		lua_pushnumber(L,-100000.0);
		lua_pushnumber(L,0.0);
	}
	double a=pow((double)PlayerData[n].ChipCount,1.0/3.0)/2.0;
	if (!World->haveArm) a = 0;
	double x=PlayerData[n].X[chipNo].x+(rand()%10000-5000)/1000.0*a+sin(randTime/150.0)*a-sin(randTime/350.0)*a;
	double y=PlayerData[n].X[chipNo].y+(rand()%10000-5000)/1000.0*a+sin(randTime/160.0)*a-sin(randTime/360.0)*a;
	double z=PlayerData[n].X[chipNo].z+(rand()%10000-5000)/1000.0*a+sin(randTime/140.0)*a-sin(randTime/340.0)*a;
	lua_pushnumber(L,x);
	lua_pushnumber(L,y);
	lua_pushnumber(L,z);
	return 3;
}
//---------------------
//--------
static unsigned long xorShift_data[4]={123456789,362436069,521288629,88675123};
static unsigned long xorShift(){ 
	unsigned long temp; 
	temp=(xorShift_data[0]^(xorShift_data[0]<<11));
	xorShift_data[0]=xorShift_data[1];
	xorShift_data[1]=xorShift_data[2];
	xorShift_data[2]=xorShift_data[3];
	return( xorShift_data[3]=(xorShift_data[3]^(xorShift_data[3]>>19))^(temp^(temp>>8)) ); 
}
static void init_xorShift(unsigned long seed){
	xorShift_data[0]=seed;
	xorShift_data[1]=!seed;
	xorShift_data[2]=seed^(seed<<13);
	xorShift_data[3]=seed^(seed<<3);
	for(int i=0;i<40;i++) xorShift();
}
//--------
int luaRand(lua_State *L)
{
	lua_pushnumber(L,xorShift());
	return 1;
}
int luaRandInit(lua_State *L)
{
	init_xorShift((unsigned long)lua_tonumber(L, 1));
	return 1;
}
int luaGetHostName(lua_State *L)
{
	lua_pushstring(L,HostName);
	return 1;
}
int luaGetPortNumber(lua_State *L)
{
	lua_pushnumber(L,PortNo);
	return 1;
}
int luaGetVertices(lua_State *L)
{
	lua_pushnumber(L,NumVertice);
	return 1;
}
int luaGettimeGetTime(lua_State *L)
{
	lua_pushnumber(L,timeGetTime());
	return 1;
}
int luaGetFaceData(lua_State *L) //({�ʔԍ�...}) return {{faceNum,vertex[3][3],normal[3],friction{Ux,Us,Ud},matrial}...}
{
	lua_newtable(L); //lua_createtable�g�������������ŗ���ð��ق̒��������� ����ȑO��Lua5.0��cratetable��������
	if(lua_istable(L, 1)){
		for(unsigned int i=1;i<=NumFace;i++){
			lua_newtable(L);
			//---------------
			lua_pushstring(L,"faceNum");
			lua_rawgeti(L,1,i);
			int isnumber=lua_isnumber (L, -1);
			unsigned int faces=(unsigned int)lua_tonumber(L, -1);
			if(!isnumber || faces<0 || faces>=NumFace){
				lua_settop(L,-4);
				break;
			}
			lua_settable(L,-3);
			//---------------
			lua_pushstring(L,"vertex");
			lua_newtable(L);
			for(int j=0;j<3;j++){
				lua_newtable(L);
				for(int k=0;k<3;k++){
					lua_pushnumber(L,(double)World->Land->Face[faces].Vertex[j][k]);
					lua_rawseti(L,-2,k+1);
				}
				lua_rawseti(L,-2,j+1);
			}
			lua_settable(L,-3);
			//---------------
			lua_pushstring(L,"normal");
			lua_newtable(L);
			for(int j=0;j<3;j++){
				lua_pushnumber(L,(double)World->Land->Face[faces].Normal[j]);
				lua_rawseti(L,-2,j+1);
			}
			lua_settable(L,-3);
			//---------------
			lua_pushstring(L,"friction");
			lua_newtable(L);
			lua_pushstring(L,"Ux");
			lua_pushnumber(L,(double)World->Land->Face[faces].Ux);
			lua_settable(L,-3);
			lua_pushstring(L,"Us");
			lua_pushnumber(L,(double)World->Land->Face[faces].Us);
			lua_settable(L,-3);
			lua_pushstring(L,"Ud");
			lua_pushnumber(L,(double)World->Land->Face[faces].Ud);
			lua_settable(L,-3);
			lua_settable(L,-3);
			//---------------
			lua_pushstring(L,"material");
			lua_pushnumber(L,(double)World->Land->Face[faces].AttribNo);
			lua_settable(L,-3);
			//---------------
			lua_rawseti(L,-2,i);
		}
	}
	return 1;
}
int luaGetPlayerExtTime(lua_State *L) //�⊮����
{
	int n=(int)lua_tonumber(L, 1);
	if(n<0 || n>=DPlay->GetNumPlayers()) {
		lua_pushnumber(L,0.0);
		return 1;
	}
	lua_pushnumber(L,(double)frameGetTime-PlayerData[n].rtime2);
	return 1;
}
int luaGetRange(lua_State *L) //�n�`�܂ł̋���
{
	int no=(int)lua_tonumber(L, 1);
	if(no<0 || no>=ChipCount){
		lua_pushnumber(L,-100000.0f);
		return 1;
	}
	GVector vec;
	int n=lua_gettop(L);
	static DWORD LastFrameTime=0;
	if(n>1 && frameGetTime!=LastFrameTime && Chip[no]->ChipType==GT_CORE){
		LastFrameTime=frameGetTime;
		vec=GVector((GFloat)lua_tonumber(L, 2),(GFloat)lua_tonumber(L, 3),(GFloat)lua_tonumber(L, 4));
	}else if(Chip[no]->ChipType!=GT_COWL){
		switch(Chip[no]->Dir) {
			case 1:vec=GVector(1,0,0);break;
			case 2:vec=GVector(0,0,-1);break;
			case 3:vec=GVector(-1,0,0);break;
			default:vec=GVector(0,0,1);break;
		}
	}else{
		lua_pushnumber(L,-100000.0f);
		return 1;
	}
	vec=vec*Chip[no]->R;
	D3DXVECTOR3 v1,v2;
	v1.x=(FLOAT)Chip[no]->X.x;
	v1.y=(FLOAT)Chip[no]->X.y;
	v1.z=(FLOAT)Chip[no]->X.z;
	v2.x=(FLOAT)vec.x;
	v2.y=(FLOAT)vec.y;
	v2.z=(FLOAT)vec.z;
	
	if(m_pLandMesh==NULL) {
		lua_pushnumber(L,-100000.0f);
		return 1;
	}
	BOOL hit;
	FLOAT dist;
	D3DXIntersect(m_pLandMesh->GetSysMemMesh(),&v1,&v2,&hit,NULL,NULL,NULL,&dist,NULL,NULL);
	if(!hit) dist=-100000.0f;
	
	lua_pushnumber(L,dist);
	return 1;
}
int luaGetRangeChip(lua_State *L) //ڲ�ƌ�����������(x,y,z,vx,vy,vz,flagAllhit)xyz:�ʒu,vxyz:ڲ�̌���,flagAllhit:false-�Ŋ�������|true-�S˯�ؽ� return chipno,dist,u,v or {{chipno,dist,u,v}...}
{
	GFloat x=(GFloat)lua_tonumber(L, 1);
	GFloat y=(GFloat)lua_tonumber(L, 2);
	GFloat z=(GFloat)lua_tonumber(L, 3);
	GFloat vx=(GFloat)lua_tonumber(L, 4);
	GFloat vy=(GFloat)lua_tonumber(L, 5);
	GFloat vz=(GFloat)lua_tonumber(L, 6);
	int flagAllhit=lua_toboolean(L, 7);
	
	if(flagAllhit){
		lua_newtable(L);
	}
	
	GVector pos=GVector(x,y,z);//ڲ�̌��_
	GVector vec=GVector(vx,vy,vz);//ڲ�̌���
	GFloat vec_abs=vec.abs();
	int chipNo=-1;
	GFloat dist=10000000.0f;
	GFloat u=0;
	GFloat v=0;
	int hitCount=0;
	for(int i=0;i<ChipCount;i++){
		GMatrix *R=&(Chip[i]->R);
		GVector normalX=GVector(R->elem[0][0],R->elem[0][1],R->elem[0][2]);
		GVector normalY=GVector(R->elem[1][0],R->elem[1][1],R->elem[1][2]);
		GVector normalZ=GVector(R->elem[2][0],R->elem[2][1],R->elem[2][2]);
		GFloat chipDotX=(Chip[i]->X).dot(normalX);
		GFloat chipDotY=(Chip[i]->X).dot(normalY);
		GFloat chipDotZ=(Chip[i]->X).dot(normalZ);
		GVector crossPoint;
		GFloat crossDistNorm=pos.pointOnFaceAndLine3(normalY,chipDotY,vec,crossPoint);
		GFloat crossDist=crossDistNorm*vec_abs;
		if(crossDistNorm<0 || crossDistNorm>1.0 || crossDist>=dist&&!flagAllhit) continue;
		GFloat crossU=crossPoint.distanceFromFace(normalX,chipDotX);
		if(fabs(crossU)>CHIPSIZE/2) continue;
		GFloat crossV=crossPoint.distanceFromFace(normalZ,chipDotZ);
		if(fabs(crossV)>CHIPSIZE/2) continue;
		
		hitCount++;
		if(flagAllhit){
			lua_newtable(L);
			lua_pushnumber(L,i);
			lua_rawseti(L,-2,1);
			lua_pushnumber(L,crossDist);
			lua_rawseti(L,-2,2);
			lua_pushnumber(L,crossU);
			lua_rawseti(L,-2,3);
			lua_pushnumber(L,crossV);
			lua_rawseti(L,-2,4);
			lua_rawseti(L,-2,hitCount);
		}
		if(crossDist<dist){
			chipNo=i;
			dist=crossDist;
			u=crossU;
			v=crossV;
		}
	}
	if(flagAllhit) return 1;
	lua_pushnumber(L,chipNo);
	lua_pushnumber(L,dist);
	lua_pushnumber(L,u);
	lua_pushnumber(L,v);
	return 4;
}
//---------------------��ߌ떂�����p��а�֐�
int luaDummyFunc0(lua_State *L)
{
	return 0;
}
int luaDummyFunc1(lua_State *L)
{
	lua_pushnumber(L, 0 );
	return 1;
}
int luaDummyFuncFunc(lua_State *L)
{
	lua_pushcfunction (L, luaDummyFunc0 );
	return 1;
}
int luaDummyFuncStr(lua_State *L)
{
	lua_pushstring(L,"");
	return 1;
}
//---------------------��ص�֐�
int luaGetNoiseTicks(lua_State *L)
{
	lua_pushnumber(L, randTime );
	return 1;
}
int luaSetScriptFunction(lua_State *L)
{
	char *name=(char*)lua_tostring(L, 1);
	lua_CFunction func=lua_tocfunction(L, 2);
	
	if((name==NULL) | (func==NULL) | (ScriptL==NULL))return 1;
	lua_register(ScriptL, name, func);
	return 1;
}
int luaGetLastChat(lua_State *L)
{
	lua_pushstring(L,LastChatData);
	return 1;
}
int luaGetView(lua_State *L)
{
	lua_pushnumber(L,UserEyePos.x);
	lua_pushnumber(L,UserEyePos.y);
	lua_pushnumber(L,UserEyePos.z);
	lua_pushnumber(L,UserRefPos.x);
	lua_pushnumber(L,UserRefPos.y);
	lua_pushnumber(L,UserRefPos.z);
	return 6;
}
int luaGetViewUp(lua_State *L)
{
	lua_pushnumber(L,UserUpVec.x);
	lua_pushnumber(L,UserUpVec.y);
	lua_pushnumber(L,UserUpVec.z);
	return 3;
}
int luaGetViewType(lua_State *L)
{
	lua_pushnumber(L,ViewType);
	return 1;
}
int luaGetViewZoom(lua_State *L)
{
	lua_pushnumber(L,Zoom);
	return 1;
}
int luaSetView(lua_State *L)
{
	UserEyePos.x=(GFloat)lua_tonumber(L, 1);
	UserEyePos.y=(GFloat)lua_tonumber(L, 2);
	UserEyePos.z=(GFloat)lua_tonumber(L, 3);
	UserRefPos.x=(GFloat)lua_tonumber(L, 4);
	UserRefPos.y=(GFloat)lua_tonumber(L, 5);
	UserRefPos.z=(GFloat)lua_tonumber(L, 6);
	UserUpVec=GVector(0,1,0);
	GVector u=(UserRefPos-UserEyePos).cross(UserUpVec).cross(UserRefPos-UserEyePos).normalize2();
	if(u.abs()!=0) UserUpVec=u;
	else UserUpVec=GVector(0,0,-1);
	ViewType=-1;
	ViewUpdate=1;
	return 0;
}
int luaSetViewUp(lua_State *L)
{
	UserUpVec.x=(GFloat)lua_tonumber(L, 1);
	UserUpVec.y=(GFloat)lua_tonumber(L, 2);
	UserUpVec.z=(GFloat)lua_tonumber(L, 3);
	ViewType=-1;
	ViewUpdate=1;
	return 0;
}
int luaSetViewType(lua_State *L)
{
	ViewType=(int)lua_tonumber(L,1);
	return 0;
}
int luaSetViewZoom(lua_State *L)
{
	Zoom=(GFloat)lua_tonumber(L,1);
	ViewUpdate=1;
	return 0;
}
int luaSetSpeedLimit(lua_State *L)
{
	int n=lua_gettop(L);
	if(n==1) GSPEEDLIMIT=lua_tonumber(L, 1);
	else if(n==2) GSPEEDLIMIT=lua_tonumber(L, 2); //��ߌ݊��p
	return 0;
}
int luaGetSpeedLimit(lua_State *L)
{
	lua_pushnumber(L,GSPEEDLIMIT);
	return 1;
}
int luaSetFogRange(lua_State *L)
{
	GFARMAX=lua_tonumber(L, 1);
	return 0;
}
int luaGetFogRange(lua_State *L)
{
	lua_pushnumber(L,GFARMAX);
	return 1;
}
int luaSetFogColor(lua_State *L)
{
	DWORD color=(DWORD)lua_tonumber(L,1);
	SetFogColor(color);
	return 0;
}
int luaGetFogColor(lua_State *L)
{
	DWORD R8G8B8=GetFogColor();
	lua_pushnumber(L,R8G8B8);

	D3DXVECTOR3 lightColor=GetLightColor();
	int r,g,b;
	r=(R8G8B8>>16)&0xFF;
	g=(R8G8B8>>8)&0xFF;
	b=(R8G8B8>>0)&0xFF;
	DWORD R8G8B8_ambient=(int)(r*lightColor.x)<<16|(int)(g*lightColor.y)<<8|(int)(b*lightColor.z);
	lua_pushnumber(L,R8G8B8_ambient);

	return 2;
}
int luaSetMakerSize(lua_State *L)
{
	GMARKERSIZE=lua_tonumber(L, 1);
	return 0;
}
int luaGetMakerSize(lua_State *L)
{
	lua_pushnumber(L,GMARKERSIZE);
	return 1;
}
int luaSetNameSize(lua_State *L)
{
	GNAMESIZE=lua_tonumber(L, 1);
	return 0;
}
int luaGetNameSize(lua_State *L)
{
	lua_pushnumber(L,GNAMESIZE);
	return 1;
}
