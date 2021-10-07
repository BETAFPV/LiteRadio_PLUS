#include "configure.h"
#include "mavlink.h"
#include "stdbool.h"


typedef struct Mavlink_Messages_t {

    int sysid;
    int compid;

    mavlink_heartbeat_t heartbeat;

} Mavlink_Messages;

Mavlink_Messages currentMsg;


uint8_t version =0;
uint8_t protocol=0;

uint8_t buff[20]={0};
uint8_t Length = 0;
uint8_t device=0;


/**********************************************************************************************************
*函 数 名: mavlinkDecode
*功能说明: 消息解析
*形    参: 接收数据
*返 回 值: 无
**********************************************************************************************************/
void mavlinkDecode(uint8_t data)
{
    static mavlink_message_t msg;
    static mavlink_status_t  status;

    if(mavlink_parse_char(0, data, &msg, &status) == false)
        return;
    
    switch(msg.msgid)
    {
        case MAVLINK_MSG_ID_heartbeat:
            mavlink_msg_heartbeat_decode(&msg,&currentMsg.heartbeat);
            version = currentMsg.heartbeat.version;
            protocol = currentMsg.heartbeat.protocol;
            device = currentMsg.heartbeat.device;
            break;
        
        default:
            break;
    }
}




