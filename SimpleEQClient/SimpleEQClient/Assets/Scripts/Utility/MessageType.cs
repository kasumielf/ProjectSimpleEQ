﻿public enum MessageType
{
    AFTER_LOGIN_SCENE_CHANGE,
    AFTER_LOGIN_FAILED,
    AFTER_CREATE_ACCOUNT_SUCCESS,
    AFTER_CREATE_ACCOUNT_FAILED,

    AFTER_RECV_CHANNEL_LIST,

    AFTER_ENTER_CHANNEL_SUCCESS,
    AFTER_ENTER_CHANNEL_FAILED,

    SCENE_CHANGE_LOGIN_TO_LOBBY,
    SCENE_CHANGE_LOBBY_TO_WORLD,

    UPDATE_FORECAST_DEBUG_INFO,
    UPDATE_FORECAST_HUD_INFO,
    UPDATE_USER_POSITION,
}