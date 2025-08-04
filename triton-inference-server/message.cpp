
// 单目标航迹
typedef struct NetTrackItem
{
        //[13]
        uint8 			status			: 4;    // 目标状态 "0-丢失 1-跟踪 2-记忆"
        uint8 			working			: 4;    // 工作方式  0-工作 1-重演
        uint8			netReport_5779	: 4;	//  5779目标上报
        uint8			netReport_wrj	: 4;	//  无人机目标上报
                                                //[14]
        uint16          tgt_num;				// 批号信息	目标批号
                                                //[15]
        uint16          chan_num;				// 目标通道号
                                                //[16]~[17]
        uint32 			burst_num;				// 目标流水号
                                                //[18]
        uint16			trk_hits;				// 航迹历史
                                                //[19]
        uint16          iffProperty : 4;		// 敌我属性	0-敌 1-敌方同盟 2-我 3-我友方 4-中立 5-不明 6-未识别
        uint16          tgt_type : 4;			// 航迹类型	0-水平 1-垂直 2-融合
        uint16			tgt_quality : 4;		// 航迹质量	0-7
        uint16			bak2 : 4;				// 备份
                                                //[20]
        uint16          fix_flag : 1;			// 固定目标：0-无效 1-有效
        uint16          ghost_flag : 1;			// 仙波	0-无效 1-有效
        uint16          slow_flag : 1;			// 慢速目标 	0-无效 1-有效
        uint16          spare3 : 13;			// 备份
                                                //[21]
        uint16			spare4;					// 备份
                                                //[22]~[23]
        uint32 			date;					// 时间信息	基日
                                                //[24]~[25]
        uint32 			time;					// 时间	当日的北京时，无符号整形,量化单位25us
                                                //[26~[27]]
        uint32   		tgt_rng;				// 滤波位置信息	径向距离	无符号整形,量化单位0.1m
                                                //[28]~[29]
        uint32   		tgt_azi; 				// 方位	无符号整形,量化单位:0.00001度
                                                //[30]~[31]
        int32   		tgt_ele; 				// 俯仰	有符号整形,量化单位:0.00001度
                                                //[32]~[33]
        uint32   		dtc_rng; 				// 点迹位置信息	径向距离	无符号整形,量化单位0.1m
                                                //[34]~[35]
        uint32   		dtc_azi; 				// 方位	无符号整形,量化单位:0.00001度
                                                //[36]~[37]
        int32   		dtc_ele; 				// 俯仰	有符号整形,量化单位:0.00001度
                                                //[38]~[39]
        int32           radial_vel;				// 速度信息	径向速度	有符号整形，量化单位0.01m/s
                                                //[40]
        int16   		azi_vel;				// 方位速度	有符号整形，量化单位: 0.001度/s
                                                //[41]
        int16   		ele_vel;				// 俯仰速度	有符号整形，量化单位: 0.001度/s
                                                //[42]~[43]
        uint32   		speed;					// 全速度 无符号整形无符号整形,量化单位0.1m
                                                //[44]
        uint16          acc;					// 空间加速度 无符号整形，0.01米/秒秒
                                                //[45]
        uint16          course;					// 航向 无符号整形，0.1度
                                                //[46]
        int16 	 		rng_err_mean;			// 距离误差均值 有号整形,量化单位0.1m
                                                //[47]
        uint16          rng_err_std;			// 距离误差均方根	无符号整形,量化单位0.1m
                                                //[48]
        int16 	 		az_err_mean;			// 方位误差均值 有符号号整形,量化单位0.001度
                                                //[49]
        uint16          az_err_std;				// 方位误差均方根, 无符号整形,量化单位0.001度
                                                //[50]
        int16 	 		ele_err_mean;			// 俯仰误差均值		有号整形,量化单位0.001度
                                                //[51]
        uint16          ele_err_std;			// 俯仰误差均方根		无符号整形,量化单位0.001度
                                                //[52]
        uint16          amp;					// 幅度信息		无符号整形，量化单位0.1dB
                                                //[53]
        uint16          snr;					// 目标信噪比		无符号整形，量化单位: 0.01dB
                                                //[54]
        int16	 		rcs;					// RCS		有符号整形，量化单位: 0.01dB
                                                //[55]
        uint16			tgt_category : 8;		// 识别信息大类		1-鸟类, 2-空飘球，3-飞机，4-汽车，5-大鸟，6-小鸟，7-无人机，0xf-不明
        uint16			tgt_species : 8;		// 识别信息小类
                                                //[56]
        uint16			tgt_threat : 8;			// 威胁度
        uint16			task_stat : 8;			// 任务计划执行状态
        uint32			plat_lon;			// [57-58] 站址信息 经度
        uint32			plat_lat;			// [59-60] 站址信息 纬度
        uint32			plat_alt;			// [61-62] 站址信息 高度
        uint16			svo_yaw;			// [63] 伺服信息	天线方位	量化单位:0.005493247
        uint16			svo_pitch;			// [64] 天线俯仰	量化单位:0.005493247
        uint16			pluse_width;		// [65] 信号形式	脉宽
        uint8			freq_ratio;			// [66] 调频斜率
        uint8			work_band;			// [66] 带宽
        uint32			disAirport;			// [67-68] 距离飞机跑到中心距离
        uint16			tas_freq;			// [69] 跟踪频点
        uint16			tas_prd;			// [70] 跟踪数据率 无符号整形,量化单位:0.001秒
        uint32			tas_num;			// [71-72] 数据处理序列号
        int32			tgtX;				// [73-74] 目标地心X,量化单位0.01
        int32			tgtY;				// [75-76] 目标地心Y,量化单位0.01
        int32			tgtZ;				// [77-78] 目标地心Z,量化单位0.01
        int32			tgtVX;				// [79-80] 地心VX速度,量化单位0.01
        int32			tgtVY;				// [81-82] 地心VY速度,量化单位0.01
        int32			tgtVZ;				// [83-84] 地心VZ速度,量化单位0.01
        uint32			threadDis;			// [85-86] 距离最近威胁区距离，单位0.1m
        uint32			threadTime;			// [87-88] 距离最近威胁区时间，单位1s
        uint16			bak3[4];			// [89-92] 
}NetTrackItem_t;


typedef struct OcdHead {
        enum {
                HeadFlag = 0xA1A1
        };

        uint16 	msg_code;                   // [0] 报文头
        uint16	majorCommand;				// [1] 命令类型大类
        uint16	msg_len;                    // [2] 帧长, 包含帧头和尾
        uint16 	msg_index;                  // [3] 帧序列号
        uint16 	rdr_station_id;             // [4] 雷达站号
        uint16 	rdr_id;                     // [5] 雷达号, 0-方位 1-俯仰 2-融合航迹
                                            // [6-8] BCD码
        uint8	year;
        uint8	month;
        uint8	day;
        uint8	hour;
        uint8	minute;
        uint8	second;

        uint16  millisecond25;				// [9] 量化单位25us
                                            // [10] 唯一标识，只有在同步时有用
                                            // 发送同步控制命令时为1，非同步控制命令（用户操控）为0
        uint16	uniqueID;				
        uint16	spare;					// [11] 备份
} OcdHead_t;

// 航迹报文标识0x1010
typedef struct NetTrackInfo
{
        OcdHead_t       header;             // 报文头
        uint16          tgt_num;            // 本帧传送目
        NetTrackItem    *track_item;		// 单航迹信息，不超过1000
        uint16			check_sum;          // 校验和
        uint16			msg_end;            // 帧尾
}NetTrackInfo_t;




// 数据大小<一个航迹数据量 退出
if (size < sizeof(OcdHead_t) + sizeof(NetTrackItem_t) + 6)
{
        return;
}

char *p = pData;
OcdHead_t header;
memcpy(&header, p, sizeof(OcdHead_t));
p += sizeof(OcdHead_t);

unsigned short tgtNum = 0;
memcpy(&tgtNum, p, 2);
//if ((tgtNum <= 0) || (tgtNum > PPI::kMaxTrackNum))
if ((tgtNum <= 0) || (tgtNum > TrackFile::Inst()->m_kMaxTrackNum))
{
        return;
}
p += 2;

for (int i = 0; i < tgtNum; ++i)
{
        TrackItem newItem;
        newItem.rdr_id = header.rdr_id;

        memset(&newItem, 0x0, sizeof(TrackItem));

        NetTrackItem_t netTrackItem;
        memcpy(&netTrackItem, p, sizeof(NetTrackItem_t));

        // 站点、批号
        newItem.dot.radarNum = header.rdr_station_id;
        newItem.dot.ph = netTrackItem.tgt_num;
        newItem.dot.serialNo = netTrackItem.burst_num;

        if (RECREP::Replaying == RecRepManager::Inst()->GetState())
        {
                bool bFind = false;
                auto listPH = RecRepManager::Inst()->GetFilterPH();
                for (auto Ph : listPH)
                {
                        if (Ph == newItem.dot.ph)
                        {
                                bFind = true;
                                break;
                        }
                }
                if (listPH.size() > 0 && !bFind)
                {
                        p += sizeof(NetTrackItem);
                        continue;
                }
        }

        // 站址
        newItem.dot.platLon = netTrackItem.plat_lon * 0.00001;	// 经纬度量化单位
        newItem.dot.platLat = netTrackItem.plat_lat * 0.00001;
        newItem.dot.platHei = netTrackItem.plat_alt * 0.01;

        // 航迹点数据
        newItem.dot.trkSource = netTrackItem.tgt_type;
        newItem.dot.iffProperty = netTrackItem.iffProperty;
        newItem.dot.dis = netTrackItem.tgt_rng * 0.1;
        newItem.dot.azi = netTrackItem.tgt_azi * 0.00001;
        newItem.dot.ele = netTrackItem.tgt_ele * 0.00001;
        newItem.dot.prjDis = newItem.dot.dis * cos(newItem.dot.ele * 3.1415926 / 180.0);
        newItem.dot.disAirport = netTrackItem.disAirport * 0.1;
        newItem.dot.speed = netTrackItem.speed * 0.1;
        newItem.dot.radialSpeed = netTrackItem.radial_vel * 0.01;
        newItem.dot.status = netTrackItem.status;
        newItem.dot.dotReport_5779 = netTrackItem.netReport_5779;
        newItem.dot.dotReport_wrj = netTrackItem.netReport_wrj;
        newItem.dot.threatLevel = netTrackItem.tgt_threat;
        newItem.dot.rcs = netTrackItem.rcs * 0.01;
        newItem.dot.snr = netTrackItem.snr * 0.01;
        newItem.dot.tgtType = netTrackItem.tgt_category;

        // 光电需要的信息
        newItem.dot.azi_vel = netTrackItem.azi_vel * 0.001;
        newItem.dot.ele_vel = netTrackItem.ele_vel * 0.001;
        newItem.dot.rng_err_mean = netTrackItem.rng_err_mean * 0.1;
        newItem.dot.rng_err_std = netTrackItem.rng_err_std * 0.1;
        newItem.dot.az_err_mean = netTrackItem.az_err_mean * 0.001;
        newItem.dot.az_err_std = netTrackItem.az_err_std * 0.001;
        newItem.dot.ele_err_mean = netTrackItem.ele_err_mean * 0.001;
        newItem.dot.ele_err_std = netTrackItem.ele_err_std * 0.001;

        // 地心xyz值转经纬高, 目标高度-站址高度
        newItem.geoVec = osg::Vec3d(netTrackItem.tgtX * 0.01, netTrackItem.tgtY * 0.01, netTrackItem.tgtZ * 0.01);
        CommGeoUtil::Inst()->EarthXYZ2LLH(newItem.geoVec, newItem.llhVec);
        newItem.dot.hei = abs(newItem.llhVec[2] - newItem.dot.platHei);

        // 将经纬高转换为世界坐标系
        MapCoordTrans::Inst()->DegreeLLH2XYZ(newItem.llhVec, newItem.mapXYZPos);

        // 距离最近威胁区时间和距离
        newItem.dot.threatAreaDis = netTrackItem.threadDis * 0.1;
        newItem.dot.threatAreaTime = netTrackItem.threadTime;

        // 鸟种编号
        newItem.dot.birdNum = netTrackItem.tgt_species;

        if (newItem.dot.status == 0)
        {
                TrackFile::Inst()->DelTrackByPH(newItem.dot.ph);
        }
        else if (newItem.dot.status == 1 || newItem.dot.status == 2)
        {
                TrackFile::Inst()->SaveTrack(newItem);
        }
        else
        {
                // 目标状态为其他值不处理
                // 此处不能return,如果return导致后面的航迹无法解析
                //qDebug() << "processTrack status = " << newItem.dot.status << " do not handle";
        }
        p += sizeof(NetTrackItem);
}
