code

equ trap_Print                        -1
equ trap_Error                        -2
equ trap_Milliseconds                 -3
equ trap_Cvar_Register                -4
equ trap_Cvar_Update                  -5
equ trap_Cvar_Set                     -6
equ trap_Cvar_VariableStringBuffer    -7
equ trap_Argc                         -8
equ trap_Argv                         -9
equ trap_Args                         -10
equ trap_FS_FOpenFile                 -11
equ trap_FS_Read                      -12
equ trap_FS_Write                     -13 
equ trap_FS_FCloseFile                -14
equ trap_SendConsoleCommand           -15
equ trap_AddCommand                   -16
equ trap_SendClientCommand            -17
equ trap_UpdateScreen                 -18
equ trap_CM_LoadMap                   -19
equ trap_CM_NumInlineModels           -20
equ trap_CM_InlineModel               -21
equ trap_CM_LoadModel                 -22
equ trap_CM_TempBoxModel              -23
equ trap_CM_PointContents             -24
equ trap_CM_TransformedPointContents  -25
equ trap_CM_BoxTrace                  -26
equ trap_CM_TransformedBoxTrace       -27
equ trap_CM_MarkFragments             -28
equ trap_S_StartSound                 -29
equ trap_S_StartLocalSound            -30
equ trap_S_ClearLoopingSounds         -31
equ trap_S_AddLoopingSound            -32
equ trap_S_UpdateEntityPosition       -33
equ trap_S_Respatialize               -34
equ trap_S_RegisterSound              -35
equ trap_S_StartBackgroundTrack       -36
equ trap_R_LoadWorldMap               -37
equ trap_R_RegisterModel              -38
equ trap_R_RegisterSkin               -39
equ trap_R_RegisterShader             -40
equ trap_R_ClearScene                 -41
equ trap_R_AddRefEntityToScene        -42
equ trap_R_AddPolyToScene             -43
equ trap_R_AddLightToScene            -44
equ trap_R_RenderScene                -45
equ trap_R_SetColor                   -46
equ trap_R_DrawStretchPic             -47
equ trap_R_ModelBounds                -48
equ trap_R_LerpTag                    -49
equ trap_GetGlconfig                  -50
equ trap_GetGameState                 -51
equ trap_GetCurrentSnapshotNumber     -52
equ trap_GetSnapshot                  -53
equ trap_GetServerCommand             -54
equ trap_GetCurrentCmdNumber          -55
equ trap_GetUserCmd                   -56
equ trap_SetUserCmdValue              -57
equ trap_R_RegisterShaderNoMip        -58
equ trap_MemoryRemaining              -59
equ trap_R_RegisterFont               -60
equ trap_Key_IsDown                   -61
equ trap_Key_GetCatcher               -62
equ trap_Key_SetCatcher               -63
equ trap_Key_GetKey                   -64
equ trap_Parse_AddGlobalDefine        -65
equ trap_Parse_LoadSource             -66
equ trap_Parse_FreeSource             -67
equ trap_Parse_ReadToken              -68
equ trap_Parse_SourceFileAndLine      -69
equ trap_S_StopBackgroundTrack        -70
equ trap_RealTime                     -71
equ trap_SnapVector                   -72
equ trap_RemoveCommand                -73
equ trap_R_LightForPoint              -74
equ trap_CIN_PlayCinematic            -75
equ trap_CIN_StopCinematic            -76
equ trap_CIN_RunCinematic             -77
equ trap_CIN_DrawCinematic            -78
equ trap_CIN_SetExtents               -79
equ trap_R_RemapShader                -80
equ trap_S_AddRealLoopingSound        -81
equ trap_S_StopLoopingSound           -82
equ trap_CM_TempCapsuleModel          -83
equ trap_CM_CapsuleTrace              -84
equ trap_CM_TransformedCapsuleTrace   -85
equ trap_R_AddAdditiveLightToScene    -86
equ trap_GetEntityToken               -87
equ trap_R_AddPolysToScene            -88
equ trap_R_inPVS                      -89
equ trap_FS_Seek                      -90
equ trap_FS_GetFileList               -91
equ trap_LiteralArgs                  -92
equ trap_CM_BiSphereTrace             -93
equ trap_CM_TransformedBiSphereTrace  -94
equ trap_GetDemoState                 -95
equ trap_GetDemoPos                   -96
equ trap_GetDemoName                  -97
equ trap_Key_KeynumToStringBuf        -98
equ trap_Key_GetBindingBuf            -99
equ trap_Key_SetBinding               -100

equ memset                            -201
equ memcpy                            -202
equ strncpy                           -203
equ sin                               -204
equ cos                               -205
equ atan2                             -206
equ sqrt                              -207
equ floor                             -208
equ ceil                              -209
equ testPrintInt                      -210
equ testPrintFloat                    -211


