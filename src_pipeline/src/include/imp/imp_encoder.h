/*
 * IMP Encoder func header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_ENCODER_H__
#define __IMP_ENCODER_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * IMP视频编码头文件
 */

/**
 * @defgroup IMP_Encoder
 * @ingroup imp
 * @brief 视频编码（JPEG, H264）模块，包含编码通道管理，编码参数设置等功能
 * @section enc_struct 1 模块结构
 * Encoder模块内部结构如下如：
 * @image html encoder_struct.jpg
 * 如上图所示，编码模块由若干个Group组成（在T15上支持两个Group），每个Group由编码Channel组成。
 * 每个编码Channel附带一个输出码流缓冲区，这个缓冲区由多个buffer组成。
 * @section enc_channel 2 编码Channel
 * 一个编码Channel可以完成一种协议的编码, 每个Group可以添加2个编码channel。
 * @section enc_rc 3 码率控制
 * @subsection enc_cbr 3.1 CBR
 * CBR（Constent Bit Rate）恒定比特率，即在码率统计时间内编码码率恒定。
 * 以H.264 编码为例，用户可设置maxQp，minQp，bitrate等。
 * maxQp，minQp 用于控制图像的质量范围， bitrate 用于钳位码率统计时间内的平均编码码率。
 * 当编码码率大于恒定码率时，图像QP 会逐步向maxQp 调整，当编码码率远小于恒定码率时，图像QP 会逐步向minQp 调整。
 * 当图像QP 达到maxQp 时，QP 被钳位到最大值，bitrate 的钳位效果失效，编码码率有可能会超出bitrate。
 * 当图像QP 达到minQp 时，QP 被钳位到最小值，此时编码的码率已经达到最大值，而且图像质量最好。
 * @subsection enc_FixQP 3.2 FixQP
 * Fix Qp 固定Qp 值。在码率统计时间内，编码图像所有宏块Qp 值相同，采用用户设定的图像Qp值。
 * @{
 */

/**
 * 注意：
 * direct_mode=2为双摄双直通模式。此模式下：
 * FrameSource0：主摄主通道；FrameSource3：次摄主通道。
 * 编码Group0绑定到FrameSource0，编码Group1绑定到FrameSource3。
 * 编码Channel0(h264)注册到编码Group0，用于生成主摄主码流。
 * 编码Channel1(h264)注册到编码Group1，用于生成次摄主码流。
 *
 * 如需抓取主摄主通道JPEG和次摄主通道JPEG，编码通道需分别设置为channel2和channel3，并按如下方式注册。
 * 主摄主通道：                    |--enc_chn 0 用于主摄主通道H264
 * FrameSource0 -- Encoder_Group0->
 *                                 |--enc_chn 2 用于主摄主通道JPEG,(不抓取主摄主通道JPEG，请忽略)
 *
 * 次摄主通道：                    |--enc_chn 1 用于次摄主通道H264
 * FrameSource3 -- Encoder_Group1->
 *                                 |--enc_chn 3 用于次摄主通道JPEG,(不抓取次摄主通道JPEG，请忽略)
 *
 * FrameSource1,FrameSource2,FrameSource4,FrameSource5为非直通通道，没有编码通道和Group的特殊限制。
 */

/**
 * 定义编码Channel码率控制器模式
 */
typedef enum {
	ENC_RC_MODE_FIXQP               = 0,	/**< Fixqp 模式 */
	ENC_RC_MODE_CBR                 = 1,	/**< CBR 模式 */
	ENC_RC_MODE_VBR                 = 2,	/**< VBR 模式*/
	ENC_RC_MODE_SMART               = 3,	/**< Smart 模式*/
	ENC_RC_MODE_INV                 = 4,	/**< INV 模式 */
} IMPEncoderRcMode;

/**
 * 定义编码channel帧率结构体,frmRateNum和frmRateDen经过最大公约数整除后两者之间的最小公倍数不能超过64，最好在设置之前就被最大公约数整除
 */
typedef struct {
	uint32_t	frmRateNum;				/**< 在一秒钟内的时间单元的数量, 以时间单元为单位。即帧率的分子 */
	uint32_t	frmRateDen;				/**< 在一帧内的时间单元的数量, 以时间单元为单位。即帧率的分母 */
} IMPEncoderFrmRate;

/**
 * 定义H.264编码Channel Fixqp属性结构
 */
typedef struct {
	uint32_t			qp;				/**< 帧级Qp值 */
} IMPEncoderAttrH264FixQP;

/**
 * 定义H.264编码Channel CBR属性结构
 */
typedef struct {
	uint32_t			maxQp;			/**< 编码器支持图像最大QP */
	uint32_t			minQp;			/**< 编码器支持图像最小QP */
	uint32_t			outBitRate;		/**< 编码器输出码率,以kbps为单位 */
	int					iBiasLvl;		/**< 调整I帧QP以调节I帧的图像质量及其码流大小,范围:[-3,3] */
	uint32_t			frmQPStep;		/**< 帧间QP变化步长 */
	uint32_t			gopQPStep;		/**< GOP间QP变化步长 */
	bool				adaptiveMode;	/**< 自适应模式*/
	bool				gopRelation;	/**< GOP是否关联 */
} IMPEncoderAttrH264CBR;

/**
 * 定义H.264编码Channel VBR属性结构
 */
typedef struct {
	uint32_t			maxQp;			/**< 编码器支持图像最大QP */
	uint32_t			minQp;			/**< 编码器支持图像最小QP */
	uint32_t			staticTime;		/**< 码率统计时间,以秒为单位 */
	uint32_t			maxBitRate;		/**< 编码器输出最大码率,以kbps为单位 */
    int32_t             iBiasLvl;       /**< 调整I帧QP以调节I帧的图像质量及其码流大小,范围:[-3,3] */
	uint32_t			changePos;		/**< VBR 开始调整 Qp 时的码率相对于最大码率的比例,取值范围:[50, 100] */
    uint32_t            qualityLvl;     /**< 视频质量最低水平, 范围[0-7], 值越低图像质量越高, 但码流越大, minBitRate = maxBitRate * quality[qualityLvl], 其中quality[] = {0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1} */
	uint32_t			frmQPStep;		/**< 帧间QP变化步长 */
	uint32_t			gopQPStep;		/**< GOP间QP变化步长 */
	bool				gopRelation;	/**< gop是否关联 */
} IMPEncoderAttrH264VBR;

/**
 * 定义H.264编码Channel Smart属性结构
 */
typedef struct {
	uint32_t			maxQp;			/**< 编码器支持图像最大QP */
	uint32_t			minQp;			/**< 编码器支持图像最小QP */
	uint32_t			staticTime;		/**< 码率统计时间,以秒为单位 */
	uint32_t			maxBitRate;		/**< 编码器输出最大码率,以kbps为单位 */
    int32_t             iBiasLvl;       /**< 调整I帧QP以调节I帧的图像质量及其码流大小,范围:[-3,3] */
	uint32_t			changePos;		/**< 开始调整 Qp 时的码率相对于最大码率的比例,取值范围:[50, 100] */
    uint32_t            qualityLvl;     /**< 视频质量最低水平, 范围[0-7], 值越低图像质量越高, 但码流越大. minBitRate = maxBitRate * quality[qualityLvl], 其中quality[] = {0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1} */
	uint32_t			frmQPStep;		/**< 帧间QP变化步长 */
	uint32_t			gopQPStep;		/**< gop间QP变化步长 */
	bool				gopRelation;	/**< gop是否关联 */
} IMPEncoderAttrH264Smart;

typedef struct {
	uint32_t			qp;				/**< 帧级Qp值 */
} IMPEncoderAttrH265FixQP;

typedef struct {
	uint32_t			maxQp;			/**< 编码器支持图像最大QP */
	uint32_t			minQp;			/**< 编码器支持图像最小QP */
	uint32_t			staticTime;		/**< 码率统计时间,以秒为单位 */
	uint32_t			outBitRate;		/**< 编码器输出码率,以kbps为单位 */
	int					iBiasLvl;		/**< 调整I帧QP以调节I帧的图像质量及其码流大小,范围:[-3,3] */
	uint32_t			frmQPStep;		/**< 帧间QP变化步长 */
	uint32_t			gopQPStep;		/**< GOP间QP变化步长 */
	uint32_t			flucLvl;		/**< 最大码率相对平均码率的波动等级,范围:[0,4] */
} IMPEncoderAttrH265CBR;

typedef struct {
	uint32_t			maxQp;			/**< 编码器支持图像最大QP */
	uint32_t			minQp;			/**< 编码器支持图像最小QP */
	uint32_t			staticTime;		/**< 码率统计时间,以秒为单位 */
	uint32_t			maxBitRate;		/**< 编码器输出最大码率,以kbps为单位 */
    int32_t             iBiasLvl;       /**< 调整I帧QP以调节I帧的图像质量及其码流大小,范围:[-3,3] */
	uint32_t			changePos;		/**< VBR 开始调整 Qp 时的码率相对于最大码率的比例,取值范围:[50, 100] */
    uint32_t            qualityLvl;     /**< 视频质量最低水平, 范围[0-7], 值越低图像质量越高, 但码流越大, minBitRate = maxBitRate * quality[qualityLvl], 其中quality[] = {0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1} */
	uint32_t			frmQPStep;		/**< 帧间QP变化步长 */
	uint32_t			gopQPStep;		/**< GOP间QP变化步长 */
	uint32_t			flucLvl;		/**< 最大码率相对平均码率的波动等级,范围:[0,4] */
} IMPEncoderAttrH265VBR;

typedef struct {
	uint32_t			maxQp;			/**< 编码器支持图像最大QP */
	uint32_t			minQp;			/**< 编码器支持图像最小QP */
	uint32_t			staticTime;		/**< 码率统计时间,以秒为单位 */
	uint32_t			maxBitRate;		/**< 编码器输出最大码率,以kbps为单位 */
    int32_t             iBiasLvl;       /**< 调整I帧QP以调节I帧的图像质量及其码流大小,范围:[-3,3] */
	uint32_t			changePos;		/**< 开始调整 Qp 时的码率相对于最大码率的比例,取值范围:[50, 100] */
    uint32_t            qualityLvl;     /**< 视频质量最低水平, 范围[0-7], 值越低图像质量越高, 但码流越大. minBitRate = maxBitRate * quality[qualityLvl], 其中quality[] = {0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1} */
	uint32_t			frmQPStep;		/**< 帧间QP变化步长 */
	uint32_t			gopQPStep;		/**< gop间QP变化步长 */
	uint32_t			flucLvl;		/**< 最大码率相对平均码率的波动等级,范围:[0,4] */
} IMPEncoderAttrH265Smart;

/**
 * 定义H.264编码Channel去噪属性,一经使能不能改变,但去噪类型可以动态改变;
 */
typedef struct {
	bool				enable;			/**< 是否使能去噪功能, 0:忽略,1:按当前帧类型去噪,信息损失最大,2:按I帧去噪，信息损失中等 */
	int					dnType;			/**< 去噪类型,0:忽略，不降噪,1:使用IP帧类型降噪,2:使用I帧类型降噪 */
	int					dnIQp;			/**< 去噪I帧量化参数 */
	int					dnPQp;			/**< 去噪P帧量化参数 */
} IMPEncoderAttrDenoise;

/**
 * 定义H.264编码Channel输入帧使用模式
 */
typedef enum {
	ENC_FRM_BYPASS	= 0,		/**< 顺序全使用模式-默认模式 */
	ENC_FRM_REUSED	= 1,		/**< 重复使用帧模式 */
	ENC_FRM_SKIP	= 2,		/**< 丢帧模式 */
} EncFrmUsedMode;

/**
 * 定义H.264编码Channel输入帧使用模式属性
 */
typedef struct {
	bool				enable;			/**< 是否使能输入帧使用模式 */
	EncFrmUsedMode		frmUsedMode;	/**< 输入帧使用模式 */
	uint32_t			frmUsedTimes;	/**< 在重复帧或丢帧模式下每次使用的帧间隔 */
} IMPEncoderAttrFrmUsed;

/**
 * 定义H.264编码Channel跳帧模式
 */
typedef enum {
	IMP_Encoder_STYPE_N1X			= 0,	/**< 1倍跳帧参考 */
	IMP_Encoder_STYPE_N2X			= 1,	/**< 2倍跳帧参考 */
	IMP_Encoder_STYPE_N4X			= 2,	/**< 4倍跳帧参考 */
	IMP_Encoder_STYPE_HN1_FALSE	    = 3,	/**< 高级跳帧模式：N1开放跳帧 */
	IMP_Encoder_STYPE_HN1_TRUE		= 4,	/**< 高级跳帧模式：N1封闭跳帧 */
	IMP_Encoder_STYPE_H1M_FALSE	    = 5,	/**< 高级跳帧模式：1M开放跳帧 */
	IMP_Encoder_STYPE_H1M_TRUE		= 6,	/**< 高级跳帧模式：1M封闭跳帧 */
} IMPSkipType;

/**
 * 定义H.264编码Channel帧参考类型
 */
typedef enum {
	IMP_Encoder_FSTYPE_IDR		= 0,	/**< 高级跳帧模式中的关键帧(IDR帧) */
	IMP_Encoder_FSTYPE_LBASE	= 1,	/**< 高级跳帧模式中的长期基本帧(P帧) */
	IMP_Encoder_FSTYPE_SBASE	= 2,	/**< 高级跳帧模式中的短期基本帧(P帧) */
	IMP_Encoder_FSTYPE_ENHANCE	= 3,	/**< 高级跳帧模式中的增强帧(P帧) */
} IMPRefType;

/**
 * 定义H.264编码Channel高级跳帧类型结构体
 */
typedef struct {
	IMPSkipType	    skipType;	    /**< 跳帧类型 */
	int				m;				/**< 增强帧间隔数 */
	int				n;				/**< 参考帧间隔数 */
	int				maxSameSceneCnt;/**< 同一场景占用gop最大数目,仅对H1M Skip类型有效,若设为未大于0,则m值不起作用 */
	int				bEnableScenecut;/**< 是否使能场景切换,仅对H1M Skip类型有效 */
	int				bBlackEnhance;	/**< 是否使得增强帧以空码流输出 */
} IMPEncoderAttrHSkip;

/**
 * 定义H.264编码Channel高级跳帧类型初始化结构体
 */
typedef struct {
	IMPEncoderAttrHSkip	hSkipAttr;	 /**< 高级跳帧属性 */
	IMPSkipType			maxHSkipType;/**< 需要使用的最大跳帧类型，影响rmem内存大小, N1X 到 N2X 需要分配2个参考重建帧空间, N4X 到 H1M_TRUE 需要分配3个参考重建帧空间, 请按需要的跳帧类型设定 */
} IMPEncoderAttrInitHSkip;

/**
 * 定义H.264编码Channel码率控制器码率控制模式属性
 */
typedef struct {
	IMPEncoderRcMode rcMode;						/**< RC 模式 */
	union {
		IMPEncoderAttrH264FixQP	 attrH264FixQp;		/**< H.264 协议编码Channel Fixqp 模式属性 */
		IMPEncoderAttrH264CBR	 attrH264Cbr;		/**< H.264 协议编码Channel Cbr 模式属性 */
		IMPEncoderAttrH264VBR	 attrH264Vbr;		/**< H.264 协议编码Channel Vbr 模式属性 */
		IMPEncoderAttrH264Smart	 attrH264Smart;		/**< H.264 协议编码Channel Smart 模式属性 */
		IMPEncoderAttrH265FixQP	 attrH265FixQp;		/**< 不支持 H.265协议编码 */
		IMPEncoderAttrH265CBR	 attrH265Cbr;		/**< 不支持 H.265协议编码 */
		IMPEncoderAttrH265VBR	 attrH265Vbr;		/**< 不支持 H.265协议编码 */
		IMPEncoderAttrH265Smart	 attrH265Smart;		/**< 不支持 H.265协议编码 */
	};
} IMPEncoderAttrRcMode;

/**
 * 定义H.264编码Channel码率控制器属性
 */
typedef struct {
	IMPEncoderFrmRate	        outFrmRate;		/**< 编码Channel的输出帧率（输出帧率不能大于输入帧率）*/
	uint32_t			        maxGop;			/**< gop值，必须是帧率的整数倍 */
    IMPEncoderAttrRcMode        attrRcMode;     /**< 码率控制模式属性 */
	IMPEncoderAttrFrmUsed	    attrFrmUsed;	/**< 输入帧使用模式属性 */
	IMPEncoderAttrDenoise	    attrDenoise;	/**< 去噪属性 */
	IMPEncoderAttrInitHSkip	    attrHSkip;		/**< 高级跳帧初始化属性 */
} IMPEncoderRcAttr;

/**
 * 定义H.264码流NALU类型
 */
typedef enum {
	IMP_H264_NAL_UNKNOWN		= 0,	/**< 未指定 */
	IMP_H264_NAL_SLICE		    = 1,	/**< 一个非IDR图像的编码条带  */
	IMP_H264_NAL_SLICE_DPA	    = 2,	/**< 编码条带数据分割块A */
	IMP_H264_NAL_SLICE_DPB	    = 3,	/**< 编码条带数据分割块B */
	IMP_H264_NAL_SLICE_DPC	    = 4,	/**< 编码条带数据分割块C */
	IMP_H264_NAL_SLICE_IDR	    = 5,	/**< IDR图像的编码条带 */
	IMP_H264_NAL_SEI			= 6,	/**< 辅助增强信息 (SEI) */
	IMP_H264_NAL_SPS			= 7,	/**< 序列参数集 */
	IMP_H264_NAL_PPS			= 8,	/**< 图像参数集 */
	IMP_H264_NAL_AUD			= 9,	/**< 访问单元分隔符 */
	IMP_H264_NAL_FILLER		    = 12,	/**< 填充数据 */
} IMPEncoderH264NaluType;

/**
 * 定义H.265码流NALU类型
 */
typedef enum {
    IMP_H265_NAL_SLICE_TRAIL_N      = 0,        /**< 尾随图像, 不带参考信息 */
    IMP_H265_NAL_SLICE_TRAIL_R      = 1,        /**< 尾随图像, 带参考信息 */
    IMP_H265_NAL_SLICE_TSA_N        = 2,        /**< 时域子层接入点图像, 不带参考信息 */
    IMP_H265_NAL_SLICE_TSA_R        = 3,        /**< 时域子层接入点图像, 带参考信息 */
    IMP_H265_NAL_SLICE_STSA_N       = 4,        /**< 逐步时域子层接入点图像, 不带参考信息 */
    IMP_H265_NAL_SLICE_STSA_R       = 5,        /**< 逐步时域子层接入点图像, 带参考信息 */
    IMP_H265_NAL_SLICE_RADL_N       = 6,        /**< 可解码随机接入前置图像, 不带参考信息 */
    IMP_H265_NAL_SLICE_RADL_R       = 7,        /**< 可解码随机接入前置图像, 带参考信息 */
    IMP_H265_NAL_SLICE_RASL_N       = 8,        /**< 跳过随机接入的前置图像, 不带参考信息 */
    IMP_H265_NAL_SLICE_RASL_R       = 9,        /**< 跳过随机接入的前置图像, 带参考信息 */
    IMP_H265_NAL_SLICE_BLA_W_LP     = 16,       /**< 断点连接接入, 带前置图像 */
    IMP_H265_NAL_SLICE_BLA_W_RADL   = 17,       /**< 断点连接接入, 带前置图像RADL */
    IMP_H265_NAL_SLICE_BLA_N_LP     = 18,       /**< 断点连接接入, 不带前置图像 */
    IMP_H265_NAL_SLICE_IDR_W_RADL   = 19,       /**< 即时解码刷新, 带前置图像RADL */
    IMP_H265_NAL_SLICE_IDR_N_LP     = 20,       /**< 即时解码刷新, 不带前置图像 */
    IMP_H265_NAL_SLICE_CRA          = 21,       /**< 纯随机接入, 带前置图像*/
    IMP_H265_NAL_VPS                = 32,       /**< 视频参数集 */
    IMP_H265_NAL_SPS                = 33,       /**< 序列参数集 */
    IMP_H265_NAL_PPS                = 34,       /**< 图像参数集 */
    IMP_H265_NAL_AUD                = 35,       /**< 访问单元分隔符 */
    IMP_H265_NAL_EOS                = 36,       /**< 序列结束 */
    IMP_H265_NAL_EOB                = 37,       /**< 比特流结束 */
    IMP_H265_NAL_FILLER_DATA        = 38,       /**< 填充数据 */
    IMP_H265_NAL_PREFIX_SEI         = 39,       /**< 辅助增强信息 (SEI) */
    IMP_H265_NAL_SUFFIX_SEI         = 40,       /**< 辅助增强信息 (SEI) */
    IMP_H265_NAL_INVALID            = 64,       /**< 无效NAL类型 */
} IMPEncoderH265NaluType;

/**
 * 定义H.264编码Channel码流NAL类型
 */
typedef union {
	IMPEncoderH264NaluType		h264Type;		/**< H264E NALU 码流包类型 */
	IMPEncoderH265NaluType		h265Type;		/**< H265E NALU 码流包类型, 不支持H265编码 */
} IMPEncoderDataType;

/**
 * 定义编码帧码流包结构体
 */
typedef struct {
	uint32_t	phyAddr;						/**< 码流包物理地址 */
	uint32_t	virAddr;						/**< 码流包虚拟地址 */
	uint32_t	length;							/**< 码流包长度 */
	int64_t		timestamp;						/**< 时间戳，单位us */
	bool		frameEnd;						/**< 帧结束标识 */
	IMPEncoderDataType	dataType;				/**< H.264Channel码流NAL类型 */
} IMPEncoderPack;

/**
 * 定义编码帧码流类型结构体
 */
typedef struct {
	IMPEncoderPack  *pack;				/**< 帧码流包结构 */
	uint32_t        packCount;			/**< 一帧码流的所有包的个数 */
	uint32_t        seq;				/**< 编码帧码流序列号 */
    IMPRefType      refType;            /**< 编码帧参考类型, 只针对H264 */
} IMPEncoderStream;

/**
 * 定义编码器裁剪属性，针对输入编码器的图像先做裁剪，与编码通道的尺寸进行比较再做缩放
 */
typedef struct {
    bool		enable;		/**< 是否进行裁剪,取值范围:[FALSE, TRUE],TRUE:使能裁剪,FALSE:不使能裁剪 */
    uint32_t	x;			/**< 裁剪的区域,左上角x坐标 */
    uint32_t	y;			/**< 裁剪的区域,左上角y坐标 */
    uint32_t	w;			/**< 裁剪的区域,宽 */
    uint32_t	h;			/**< 裁剪的区域,高 */
} IMPEncoderCropCfg;

/**
 * 定义编码器插入用户数据属性,只针对H264
 */
typedef struct {
	uint32_t			maxUserDataCnt;		/**< 最大用户插入数据缓存空间个数,范围：0-2 */
	uint32_t			maxUserDataSize;	/**< 最大用户插入数据缓存空间大小,范围：16-1024 */
} IMPEncoderUserDataCfg;

/**
 * 定义编码器属性结构体
 */
typedef struct {
	IMPPayloadType			enType;			/**< 编码协议类型 */
	uint32_t				bufSize;		/**< 配置 buffer 大小，取值范围:不小于图像宽高乘积的1.5倍。设置通道编码属性时，将此参数设置为0，IMP内部会自动计算大小 */
	uint32_t				profile;		/**< 编码的等级, 0: baseline; 1:MP; 2:HP */
	uint32_t				picWidth;		/**< 编码图像宽度 */
	uint32_t				picHeight;		/**< 编码图像高度 */
	IMPEncoderCropCfg		crop;			/**< 编码器裁剪属性 */
	IMPEncoderUserDataCfg	userData;		/**< 插入用户数据属性,只针对H264 */
} IMPEncoderAttr;

/**
 * 定义编码Channel属性结构体
 */
typedef struct {
	IMPEncoderAttr      encAttr;     /**< 编码器属性结构体 */
	IMPEncoderRcAttr    rcAttr;      /**< 码率控制器属性结构体,只针对H264 */
	bool                bEnableIvdc; /**< ISP VPU Direct Connect使能标志 */
} IMPEncoderCHNAttr;

/**
 * 定义编码Channel的状态结构体
 */
typedef struct {
	bool		registered;			/**< 注册到Group标志，取值范围:{TRUE, FALSE}，TRUE:注册，FALSE:未注册 */
	uint32_t	leftPics;			/**< 待编码的图像数 */
	uint32_t	leftStreamBytes;	/**< 码流buffer剩余的byte数 */
	uint32_t	leftStreamFrames;	/**< 码流buffer剩余的帧数 */
	uint32_t	curPacks;			/**< 当前帧的码流包个数 */
	uint32_t	work_done;			/**< 通道程序运行状态，0：正在运行，1，未运行 */
} IMPEncoderCHNStat;

/**
 * 定义彩转灰(C2G)参数
 */
typedef struct {
    bool	enable;			/**< 开启或关闭彩转灰功能 */
} IMPEncoderColor2GreyCfg;

/**
 * 定义H.264编码Channel设置EnableIDR参数
 */
typedef struct {
    bool	enable;			/**< 是否设置EnableIDR */
} IMPEncoderEnIDRCfg;

/**
 * 定义H.264编码Channel设置gopsize参数
 */
typedef struct {
	int		gopsize;		/**< IDR参数 */
} IMPEncoderGOPSizeCfg;

/**
 * 定义H.264编码Channel设置ROI参数
 */
typedef struct {
	uint32_t	u32Index;	/**< ROI区域索引值，支持0-7 */
	bool		bEnable;	/**< 是否使能本区域ROI功能 */
	bool		bRelatedQp;	/**< 0：绝对ROI，1：相对ROI */
	int			s32Qp;		/**< ROI区域的相对或绝对qp值 */
	IMPRect		rect;		/**< 区域坐标属性 */
} IMPEncoderROICfg;

/**
 * 定义H.264编码Channel码率控制中超大帧处理模式
 */
typedef enum {
    IMP_RC_SUPERFRM_NONE        = 0,    /**< 无特殊策略, 支持 */
    IMP_RC_SUPERFRM_DISCARD     = 1,    /**< 丢弃超大帧, 不支持, 由调用者自己决定是否丢弃 */
    IMP_RC_SUPERFRM_REENCODE    = 2,    /**< 重编超大帧, 支持 */
    IMP_RC_SUPERFRM_BUTT        = 3,
} IMPEncoderSuperFrmMode;

/**
 * 定义H.264编码Channel码率控制优先级枚举
 */
typedef enum {
    IMP_RC_PRIORITY_RDO                 = 0,    /**< 目标码率与质量平衡 */
    IMP_RC_PRIORITY_BITRATE_FIRST       = 1,    /**< 目标码率优先 */
    IMP_RC_PRIORITY_FRAMEBITS_FIRST     = 2,    /**< 超大帧阈值优先 */
    IMP_RC_PRIORITY_BUTT                = 3,
} IMPEncoderRcPriority;

/**
 * 定义H.264编码Channel超大帧处理策略参数
 */
typedef struct {
    IMPEncoderSuperFrmMode      superFrmMode;       /**< 超大帧处理模式,默认为 SUPERFRM_REENCODE */
    uint32_t                    superIFrmBitsThr;   /**< I 帧超大阈值, 默认为w*h*3/2*8/ratio, ratio: 主分辨率为6, 次分辨率为3 */
    uint32_t                    superPFrmBitsThr;   /**< P 帧超大阈值, 默认为I帧超大阈值除以1.4 */
    uint32_t                    superBFrmBitsThr;   /**< B 帧超大阈值, 默认为P帧超大阈值除以1.3, 暂不支持B帧 */
    IMPEncoderRcPriority        rcPriority;         /**< 码率控制优先级, 默认为 IMP_RC_PRIORITY_RDO */
} IMPEncoderSuperFrmCfg;

/**
 * 定义 H.264 协议编码通道色度量化结构体
 */
typedef struct {
    int         chroma_qp_index_offset;     /**< 具体含义请参见 H.264 协议, 系统默认值为 0; 取值范围:[-12, 12] */
} IMPEncoderH264TransCfg;

/**
 * 定义 H.265 协议编码通道色度量化结构体
 */
typedef struct {
    int         chroma_cr_qp_offset;     /**< 不支持H.265协议编码 */
    int         chroma_cb_qp_offset;     /**< 不支持H.265协议编码 */
} IMPEncoderH265TransCfg;

/**
 * 定义宏块级编码控制模式结构体, H264仅支持1-4
 */
typedef enum {
    ENC_QPG_CLOSE   = 0,    /**< 关闭宏块级编码控制 */
    ENC_QPG_CRP     = 1,    /**< 开启CRP模式(默认) */
    ENC_QPG_SAS     = 2,    /**< 开启SAS模式 */
    ENC_QPG_SASM    = 3,    /**< 开启SAS模式并使能多阈值模式 */
    ENC_QPG_MBRC    = 4,    /**< 开启宏块级码率控制 */
    ENC_QPG_CRP_TAB = 5,    /**< 开启CRP模式和QP_TAB */
    ENC_QPG_SAS_TAB = 6,    /**< 开启SAS模式和QP_TAB */
    ENC_QPG_SASM_TAB= 7,    /**< 开启SAS模式并使能多阈值模式和QP_TAB */
} IMPEncoderQpgMode;

/*
 MPEncoderJpegeQl*定义JPEG编码量化表参数结构体
 */
typedef struct {
    bool user_ql_en;		/**< 0: 使用默认量化表; 1:使用用户量化表*/
    uint8_t qmem_table[128];/**< 用户自定义量化表*/
} IMPEncoderJpegeQl;

/**
 * @fn int IMP_Encoder_CreateGroup(int encGroup)
 *
 * 创建编码Group
 *
 * @param[in] encGroup Group号,取值范围:[0, @ref NR_MAX_ENC_GROUPS - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 一路Group仅支持一路分辨率，不同分辨率需启动新的Group。一路Group允许最多注册2个编码channel
 *
 * @attention 如果指定的Group已经存在，则返回失败
 */
int IMP_Encoder_CreateGroup(int encGroup);

/**
 * @fn int IMP_Encoder_DestroyGroup(int encGroup)
 *
 * 销毁编码Grouop.
 *
 * @param[in] encGroup Group号,取值范围:[0, @ref NR_MAX_ENC_GROUPS - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 销毁Group时，必须保证Group为空，即没有任何Channel在Group中注册，或注册到Group中的Channel已经反注册，否则返回失败
 *
 * @attention 销毁并不存在的Group，则返回失败
 */
int IMP_Encoder_DestroyGroup(int encGroup);

/**
 * @fn int IMP_Encoder_CreateChn(int encChn, const IMPEncoderCHNAttr *attr)
 *
 * 创建编码Channel
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] attr 编码Channel属性指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 编码Channel属性由两部分组成，编码器属性和码率控制属性
 * @remark 编码器属性首先需要选择编码协议，然后分别对各种协议对应的属性进行赋值
 */
int IMP_Encoder_CreateChn(int encChn, const IMPEncoderCHNAttr *attr);

/**
 * @fn int IMP_Encoder_DestroyChn(int encChn)
 *
 * 销毁编码Channel
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @attention 销毁并不存在的Channel，则返回失败
 * @attention 销毁前必须保证Channel已经从Group反注册，否则返回失败
 */
int IMP_Encoder_DestroyChn(int encChn);

/**
 * @fn int IMP_Encoder_GetChnAttr(int encChn, IMPEncoderCHNAttr * const attr)
 *
 * 获取编码Channel的属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] attr 编码Channel属性
 *
 * @retval 0 成功
 * @retval 非0 失败
 */
int IMP_Encoder_GetChnAttr(int encChn, IMPEncoderCHNAttr * const attr);

/**
 * @fn int IMP_Encoder_RegisterChn(int encGroup, int encChn)
 *
 * 注册编码Channel到Group
 *
 * @param[in] encGroup 编码Group号,取值范围: [0, @ref NR_MAX_ENC_GROUPS - 1]
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @attention 注册并不存在的Channel，则返回失败
 * @attention 注册Channel到不存在的Group，否则返回失败
 * @attention 同一个编码Channel只能注册到一个Group，如果该Channel已经注册到某个Group，则返回失败
 * @attention 如果一个Group已经被注册，那么这个Group就不能再被其他的Channel注册，除非之前注册关系被解除
 */
int IMP_Encoder_RegisterChn(int encGroup, int encChn);

/**
 * @fn int IMP_Encoder_UnRegisterChn(int encChn)
 *
 * 反注册编码Channel到Group
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 * 
 * @remark Channel注销之后，编码Channel会被复位，编码Channel里的码流buffer都会被清空，如果用户还在使用未及时释放的码流buffer，将不能保证buffer数据的正确性
 * @remark 用户可以使用IMP_Encoder_Query接口来查询编码Channel码流buffer状态，确认码流buffer里的码流取完之后再反注册Channel
 *
 * @attention 注销未创建的Channel，则返回失败
 * @attention 注销未注册的Channel，则返回失败
 * @attention 如果编码Channel未停止接收图像编码，则返回失败
 */
int IMP_Encoder_UnRegisterChn(int encChn);

/**
 * @fn int IMP_Encoder_StartRecvPic(int encChn)
 *
 * 开启编码Channel接收图像
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 开启编码Channel接收图像后才能开始编码
 *
 * @attention 如果Channel未创建，则返回失败
 * @attention 如果Channel没有注册到Group，则返回失败
 */
int IMP_Encoder_StartRecvPic(int encChn);

/**
 * @fn int IMP_Encoder_StopRecvPic(int encChn)
 *
 * 停止编码Channel接收图像
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 此接口并不判断当前是否停止接收，即允许重复停止接收不返回错误
 * @remark 调用此接口仅停止接收原始数据编码，码流buffer并不会被消除
 *
 * @attention 如果Channel未创建，则返回失败
 * @attention 如果Channel没有注册到Group，则返回失败
 */
int IMP_Encoder_StopRecvPic(int encChn);

/**
 * @fn int IMP_Encoder_Query(int encChn, IMPEncoderCHNStat *stat)
 *
 * 查询编码Channel状态
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] stat 编码Channel状态
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 无
 *
 * @attention 无
 */
int IMP_Encoder_Query(int encChn, IMPEncoderCHNStat *stat);

/**
 * @fn int IMP_Encoder_GetStream(int encChn, IMPEncoderStream *stream, bool blockFlag)
 *
 * 获取编码的码流
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] stream 码流结构体指针
 * @param[in] blockFlag 是否使用阻塞方式获取，0：非阻塞，1：阻塞
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 每次获取一帧码流的数据
 * @remark 如果用户长时间不获取码流,码流缓冲区就会满。一个编码Channel如果发生码流缓冲区满,就会把后 \n
 * 面接收的图像丢掉,直到用户获取码流,从而有足够的码流缓冲可以用于编码时,才开始继续编码。建议用户   \n
 * 获取码流接口调用与释放码流的接口调用成对出现,且尽快释放码流,防止出现由于用户态获取码流,释放不   \n
 * 及时而导致的码流 buffer 满,停止编码。
 * @remark 对于H264类型码流，一次调用成功获取一帧的码流，这帧码流可能包含多个包。
 * @remark 对于JPEG类型码流，一次调用成功获取一帧的码流，这帧码流只包含一个包，这一帧包含了JPEG图片文件的完整信息。
 *
 * 示例：
 * @code
 * int ret;
 * ret = IMP_Encoder_PollingStream(ENC_H264_CHANNEL, 1000); //Polling码流Buffer，等待可获取状态
 * if (ret < 0) {
 *     printf("Polling stream timeout\n");
 *     return -1;
 * }
 *
 * IMPEncoderStream stream;
 * ret = IMP_Encoder_GetStream(ENC_H264_CHANNEL, &stream, 1); //获取一帧码流，阻塞方式
 * if (ret < 0) {
 *     printf("Get Stream failed\n");
 *     return -1;
 * }
 *
 * int i, nr_pack = stream.packCount;
 * for (i = 0; i < nr_pack; i++) { //保存这一帧码流的每个包
 *     ret = write(stream_fd, (void *)stream.pack[i].virAddr,
 *                stream.pack[i].length);
 *     if (ret != stream.pack[i].length) {
 *         printf("stream write error:%s\n", strerror(errno));
 *         return -1;
 *     }
 * }
 * @endcode
 *
 * @attention 如果pstStream为NULL,则返回失败；
 * @attention 如果Channel未创建，则返回失败；
 */
int IMP_Encoder_GetStream(int encChn, IMPEncoderStream *stream, bool blockFlag);

/**
 * @fn int IMP_Encoder_ReleaseStream(int encChn, IMPEncoderStream *stream)
 *
 * 释放码流缓存
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] stream 码流结构体指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 此接口应当和IMP_Encoder_GetStream配对起来使用，\n
 * 用户获取码流后必须及时释放已经获取的码流缓存，否则可能会导致码流buffer满，影响编码器编码。\n
 * 并且用户必须按先获取先释放的顺序释放已经获取的码流缓存。
 * @remark 在编码Channel反注册后，所有未释放的码流包均无效，不能再使用或者释放这部分无效的码流缓存。
 *
 * @attention 如果pstStream为NULL,则返回失败；
 * @attention 如果Channel未创建，则返回失败；
 * @attention 释放无效的码流会返回失败。
 */
int IMP_Encoder_ReleaseStream(int encChn, IMPEncoderStream *stream);

/**
 * @fn int IMP_Encoder_PollingStream(int encChn, uint32_t timeoutMsec)
 *
 * Polling码流缓存
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] timeoutMsec 超时时间，单位：毫秒
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 在获取码流之前可以用过此API进行Polling，当码流缓存不为空时或超时时函数返回。
 *
 * @attention 无
 */
int IMP_Encoder_PollingStream(int encChn, uint32_t timeoutMsec);

/**
 * @fn int IMP_Encoder_GetFd(int encChn)
 *
 * 获取编码Channel对应的设备文件句柄
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval >=0 成功, 返回设备文件描述符
 * @retval < 0 失败
 *
 * @remark 在使用IMP_Encoder_PollingStream不合适的场合，比如在同一个地方Polling多个编码channel的编码完成情况时, \n
 * 可以使用此文件句柄调用select, poll等类似函数来阻塞等待编码完成事件
 * @remark 调用此API需要通道已经存在
 *
 * @attention 无
 */
int IMP_Encoder_GetFd(int encChn);

/**
 * @fn int IMP_Encoder_SetMaxStreamCnt(int encChn, int nrMaxStream)
 *
 * 设置码流缓存Buffer个数
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] nrMaxStream 码流Buffer数,取值范围: [1, @ref NR_MAX_ENC_CHN_STREAM]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 由于码流缓存Buffer个数在通道创建时就已经固定，因此次API需要在通道创建之前调用。
 * @remark 若通道创建之前不调用此API设置码流缓存Buffer个数，则使用SDK默认的buffer个数。
 *
 * @attention 无
 */
int IMP_Encoder_SetMaxStreamCnt(int encChn, int nrMaxStream);

/**
 * @fn int IMP_Encoder_GetMaxStreamCnt(int encChn, int *nrMaxStream)
 *
 * 获取码流Buffer数
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] nrMaxStream 码流Buffer数变量指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 无
 *
 * @attention 无
 */
int IMP_Encoder_GetMaxStreamCnt(int encChn, int *nrMaxStream);

/**
 * @fn int IMP_Encoder_RequestIDR(int encChn)
 *
 * 请求IDR帧
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 在调用此API后，会在最近的编码帧申请IDR帧编码。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_RequestIDR(int encChn);

/**
 * @fn int IMP_Encoder_FlushStream(int encChn)
 *
 * 刷掉编码器里残留的旧码流，并以IDR帧开始编码
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 在调用此API后，会在最近的编码帧申请IDR帧编码。
 *
 * @attention 无
 */
int IMP_Encoder_FlushStream(int encChn);

/**
 * @fn int IMP_Encoder_SetChnColor2Grey(int encChn, const IMPEncoderColor2GreyCfg *pstColor2Grey).
 *
 * 设置彩转灰功能
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] pstColor2Grey 彩转灰功能的参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API设置通道的彩转灰功能，在下一个IDR或者P帧生效。
 * @remark 调用此API需要通道已经存在。
 *
 * @attention 无
 */
int IMP_Encoder_SetChnColor2Grey(int encChn, const IMPEncoderColor2GreyCfg *pstColor2Grey);

/**
 * @fn int IMP_Encoder_GetChnColor2Grey(int encChn, IMPEncoderColor2GreyCfg *pstColor2Grey).
 *
 * 获取彩转灰功能属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pstColor2Grey 彩转灰功能的参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会获取通道的彩转灰功能属性，调用此API需要通道已经存在。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_GetChnColor2Grey(int encChn, IMPEncoderColor2GreyCfg *pstColor2Grey);

/**
 * @fn int IMP_Encoder_SetChnAttrRcMode(int encChn, const IMPEncoderAttrRcMode *pstRcModeCfg).
 *
 * 设置码率控制模式属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] pstRcCfg 码率控制模式属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会设置通道的码率控制模式属性，下一个IDR生效,调用此API需要通道已经存在。
 *
 * @attention 目前，码率控制模式支持ENC_RC_MODE_FIXQP, ENC_RC_MODE_CBR, ENC_RC_MODE_VBR 与 ENC_RC_MODE_SMART
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_SetChnAttrRcMode(int encChn, const IMPEncoderAttrRcMode *pstRcModeCfg);

/**
 * @fn int IMP_Encoder_GetChnAttrRcMode(int encChn, IMPEncoderAttrRcMode *pstRcModeCfg).
 *
 * 获取码率控制模式属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pstRcCfg 码率控制模式属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会获取通道的码率控制模式属性，调用此API需要通道已经存在。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_GetChnAttrRcMode(int encChn, IMPEncoderAttrRcMode *pstRcModeCfg);

/**
 * @fn int IMP_Encoder_SetChnFrmRate(int encChn, const IMPEncoderFrmRate *pstFps)
 *
 * 动态设置帧率控制属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pstFpsCfg 帧率控制属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会重新设置编码器帧率属性，帧率属性在下一个GOP生效，最大延时1秒钟生效，调用此API需要通道已经存在。
 * @remark 如果调用IMP_FrameSource_SetChnFPS()函数动态改变系统帧率，那么需要调用该函数修改编码器帧率，完成正确参数配置。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_SetChnFrmRate(int encChn, const IMPEncoderFrmRate *pstFps);

/**
 * @fn int IMP_Encoder_GetChnFrmRate(int encChn, IMPEncoderFrmRate *pstFps)
 *
 * 获取帧率控制属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pstFpsCfg 帧率控制属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会获取通道的帧率控制属性，调用此API需要通道已经存在。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_GetChnFrmRate(int encChn, IMPEncoderFrmRate *pstFps);

/**
 * @fn int IMP_Encoder_SetChnROI(int encChn, const IMPEncoderROICfg *pstVencRoiCfg)
 *
 * 设置通道ROI属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pstFpsCfg ROI属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会设置通道的ROI属性，调用此API需要通道已经存在。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_SetChnROI(int encChn, const IMPEncoderROICfg *pstVencRoiCfg);

/**
 * @fn int IMP_Encoder_GetChnROI(int encChn, IMPEncoderROICfg *pstVencRoiCfg)
 *
 * 获取通道ROI属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pstFpsCfg ROI属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会获取通道的ROI属性，调用此API需要通道已经存在。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_GetChnROI(int encChn, IMPEncoderROICfg *pstVencRoiCfg);

/**
 * @fn int IMP_Encoder_GetGOPSize(int encChn, IMPEncoderGOPSizeCfg *pstGOPSizeCfg)
 *
 * 获取通道GOP属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pstGOPSizeCfg GOPSize属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会获取通道的GOPSize属性，调用此API需要通道已经存在。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_GetGOPSize(int encChn, IMPEncoderGOPSizeCfg *pstGOPSizeCfg);

/**
 * @fn int IMP_Encoder_SetGOPSize(int encChn, const IMPEncoderGOPSizeCfg *pstGOPSizeCfg)
 *
 * 设置通道GOP属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pstGOPSizeCfg GOP属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会设置通道的GOPSize属性，调用此API需要通道已经存在。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_SetGOPSize(int encChn, const IMPEncoderGOPSizeCfg *pstGOPSizeCfg);

/**
 * @fn int IMP_Encoder_SetChnFrmUsedMode(int encChn, const IMPEncoderAttrFrmUsed *pfrmUsedAttr)
 *
 * 设置通道输入帧使用模式属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pfrmUsedAttr 输入帧使用模式属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会设置通道的输入帧使用模式属性，调用此API需要通道已经存在。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_SetChnFrmUsedMode(int encChn, const IMPEncoderAttrFrmUsed *pfrmUsedAttr);

/**
 * @fn int IMP_Encoder_GetChnFrmUsedMode(int encChn, IMPEncoderAttrFrmUsed *pfrmUsedAttr)
 *
 * 获取通道输入帧使用模式属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pfrmUsedAttr 输入帧使用模式属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会获取通道的输入帧使用模式属性，调用此API需要通道已经存在。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_GetChnFrmUsedMode(int encChn, IMPEncoderAttrFrmUsed *pfrmUsedAttr);

/**
 * @fn int IMP_Encoder_SetChnDenoise(int encChn, const IMPEncoderAttrDenoise *pdenoiseAttr)
 *
 * 设置通道去噪属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] pdenoiseAttr 去噪属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会设置通道的去噪属性，调用此API需要通道已经存在。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_SetChnDenoise(int encChn, const IMPEncoderAttrDenoise *pdenoiseAttr);

/**
 * @fn int IMP_Encoder_GetChnDenoise(int encChn, IMPEncoderAttrDenoise *pdenoiseAttr)
 *
 * 获取通道去噪属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pdenoiseAttr 去噪属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会获取通道的去噪属性，调用此API需要通道已经存在。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_GetChnDenoise(int encChn, IMPEncoderAttrDenoise *pdenoiseAttr);

/**
 * @fn int IMP_Encoder_SetChnHSkip(int encChn, const IMPEncoderAttrHSkip *phSkipAttr)
 *
 * 设置通道高级跳帧属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] phSkipAttr 高级跳帧属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会设置通道高级跳帧属性，调用此API需要通道已经存在。
 * @remark 若创建通道时设置的高级跳帧类型是 IMP_Encoder_STYPE_N1X 到 IMP_Encoder_STYPE_N2X 中的一个, \n
 * 此API设置跳帧类型只能为 IMP_Encoder_STYPE_N1X 或 IMP_Encoder_STYPE_N2X 中的任意一个
 * @remark 若创建通道时设置的高级跳帧类型是 IMP_Encoder_STYPE_N4X 到 IMP_Encoder_STYPE_H1M_TRUE 中的一个，\n
 * 则可以设置为任意一个高级跳帧类型
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_SetChnHSkip(int encChn, const IMPEncoderAttrHSkip *phSkipAttr);

/**
 * @fn int IMP_Encoder_GetChnHSkip(int encChn, IMPEncoderAttrHSkip *phSkipAttr)
 *
 * 获取通道高级跳帧属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] phSkipAttr 高级跳帧属性参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会获取通道高级跳帧属性，调用此API需要通道已经存在。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_GetChnHSkip(int encChn, IMPEncoderAttrHSkip *phSkipAttr);

/**
 * @fn int IMP_Encoder_SetChnHSkipBlackEnhance(int encChn, const int bBlackEnhance)
 *
 * 设置通道高级跳帧中bBlackEnhance属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] bBlackEnhance 逻辑值，对应IMPEncoderAttrHSkip中bBlackEnhance值
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API会设置通道高级跳帧中bBlackEnhance属性，调用此API需要通道已经存在。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_SetChnHSkipBlackEnhance(int encChn, const int bBlackEnhance);

/**
 * @fn int IMP_Encoder_InsertUserData(int encChn, void *userData, uint32_t userDataLen)
 *
 * 插入用户数据
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] userData 用户数据指针
 * @param[in] userDataLen 用户数据长度, 取值范围:(0, 1024],以 byte 为单位
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此API需要通道已经存在
 * @remark 如果通道未创建,则返回失败
 * @remark 如果userData为空或userDataLen为0,则返回失败
 * @remark 插入用户数据,只支持H.264编码协议
 * @remark 最多分配2块内存空间用于缓存用户数据,且每段用户数据大小不超过1k byte。\n
 * 如果用户插入的数据多余2块,或插入的一段用户数据大于1k byte 时,此接口会返回错误。
 * @remark 每段用户数据以SEI包的形式被插入到最新的图像码流包之前。在某段用户数据包被编码发送之后, \n
 * 通道内缓存这段用户数据的内存空间被清零,用于存放新的用户数据
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_InsertUserData(int encChn, void *userData, uint32_t userDataLen);

/**
 * @fn int IMP_Encoder_SetFisheyeEnableStatus(int encChn, int enable)
 *
 * 设置Ingenic提供的鱼眼矫正算法的使能状态
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] enable 0:不使能(默认),1:使能
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 由于鱼眼矫正算法的使能状态在通道创建时就已经固定，因此次API需要在通道创建之前调用。
 * @remark 若通道创建之前不调用此API设置Ingenic提供的鱼眼矫正算法的使能状态,则默认不使能，即不能使用君正提供的鱼眼矫正算法。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_SetFisheyeEnableStatus(int encChn, int enable);

/**
 * @fn int IMP_Encoder_GetFisheyeEnableStatus(int encChn, int *enable)
 *
 * 获取Ingenic提供的鱼眼矫正算法的使能状态
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] enable 返回设置的Ingenic提供的鱼眼矫正算法的使能状态,0:未使能,1:已使能
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_GetFisheyeEnableStatus(int encChn, int *enable);

/**
 * @fn int IMP_Encoder_SetChangeRef(int encChn, int bEnable)
 *
 * 设置是否允许改变BASE帧参考方式的状态
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] enable 0：不允许改变，1，允许改变(默认)
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 此API必须在创建编码通道后调用，设置完后编码下一帧就会生效。
 * @remark 此API只适用于SMART编码方式
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_SetChangeRef(int encChn, int bEnable);

/**
 * @fn int IMP_Encoder_GetChangeRef(int encChn, int *bEnable)
 *
 * 获取是否允许改变BASE帧参考方式的状态
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] enable 返回是否允许改变BASE帧参考方式的状态，0：不允许改变，1，允许改变
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_GetChangeRef(int encChn, int *bEnable);

/**
 * @fn int IMP_Encoder_SetMbRC(int encChn, int bEnable)
 *
 * 设置是否开启宏块级qp控制
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] bEnable 0:不开启(默认), 1:开启
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 此API必须在创建编码通道后调用，设置完后编码下一帧就会生效。
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_SetMbRC(int encChn, int bEnable);

/**
 * @fn int IMP_Encoder_GetMbRC(int encChn, int *bEnable)
 *
 * 获取是否开启宏块级qp控制的状态
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] bEnable 返回是否开启宏块级qp控制的状态, 0:不开启, 1:开启
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_GetMbRC(int encChn, int *bEnable);

/**
 * @fn int IMP_Encoder_SetSuperFrameCfg(int encChn, const IMPEncoderSuperFrmCfg *pstSuperFrmParam)
 *
 * 设置编码超大帧配置
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] pstSuperFrmParam 编码超大帧配置
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 如果通道未创建,则返回失败
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_SetSuperFrameCfg(int encChn, const IMPEncoderSuperFrmCfg *pstSuperFrmParam);

/**
 * @fn int IMP_Encoder_GetSuperFrameCfg(int encChn, IMPEncoderSuperFrmCfg *pstSuperFrmParam)
 *
 * 获取编码超大帧配置
 *
 * @param[in] encChn 编码Channel号, 取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pstSuperFrmParam 返回编码超大帧配置
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 如果通道未创建, 则返回失败
 *
 * @attention 此API只适用于H264编码channel
 */
int IMP_Encoder_GetSuperFrameCfg(int encChn, IMPEncoderSuperFrmCfg *pstSuperFrmParam);

/**
 * @fn int IMP_Encoder_SetH264TransCfg(int encChn, const IMPEncoderH264TransCfg *pstH264TransCfg)
 *
 * 设置 H.264 协议编码通道的色度量化属性
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] pstH264TransCfg H.264 协议编码通道的色度量化属性
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 如果通道未创建,则返回失败
 * @remark 此API只适用于H264
 * @remark 建议在创建编码channel之后，startRecvPic之前调用, 设置时先GetH264TransCfg，然后再SetH264TransCfg
 *
 * @attention 无
 */
int IMP_Encoder_SetH264TransCfg(int encChn, const IMPEncoderH264TransCfg *pstH264TransCfg);

/**
 * @fn int IMP_Encoder_GetH264TransCfg(int encChn, IMPEncoderH264TransCfg *pstH264TransCfg)
 *
 * 获取 H.264 协议编码通道的色度量化属性
 *
 * @param[in] encChn 编码Channel号, 取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pstH264TransCfg 返回H.264 协议编码通道的色度量化属性
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 如果通道未创建, 则返回失败
 * @remark 此API只适用于H264
 *
 * @attention 无
 */
int IMP_Encoder_GetH264TransCfg(int encChn, IMPEncoderH264TransCfg *pstH264TransCfg);

/**
 * @fn int IMP_Encoder_SetQpgMode(int encChn, const IMPEncoderQpgMode *pstQpgMode)
 *
 * 设置宏块级编码模式
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] pstQpgMode 宏块级编码模式
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 如果通道未创建,则返回失败
 * @remark 此API只适用于H264
 *
 * @attention 无
 */
int IMP_Encoder_SetQpgMode(int encChn, const IMPEncoderQpgMode *pstQpgMode);

/**
 * @fn int IMP_Encoder_GetQpgMode(int encChn, IMPEncoderQpgMode *pstQpgMode)
 *
 * 获取宏块级编码模式
 *
 * @param[in] encChn 编码Channel号, 取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pstQpgMode 宏块级编码模式
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 如果通道未创建, 则返回失败
 * @remark 此API只适用于H264
 *
 * @attention 无
 */
int IMP_Encoder_GetQpgMode(int encChn, IMPEncoderQpgMode *pstQpgMode);

/**
 * @fn int IMP_Encoder_GetChnEncType(int encChn, IMPPayloadType *payLoadType)
 *
 * 获取图像编码协议类型
 *
 * @param[in] encChn 编码Channel号, 取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] payLoadType 返回获取图像编码协议类型
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 如果通道未创建, 则返回失败
 *
 * @attention 无
 */
int IMP_Encoder_GetChnEncType(int encChn, IMPPayloadType *payLoadType);

/**
 * @fn int IMP_Encoder_SetJpegeQl(int encChn, const IMPEncoderJpegeQl *pstJpegeQl)
 *
 * 设置 JPEG 协议编码通道的量化表配置参数
 *
 * @param[in] encChn 编码Channel号,取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[in] pstJpegeQl JPEG 协议编码通道的量化表配置参数,128个字节填入量化表
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 如果通道未创建,则返回失败
 * @remark 此API只适用于JPEG
 *
 * @attention 无
 */
int IMP_Encoder_SetJpegeQl(int encChn, const IMPEncoderJpegeQl *pstJpegeQl);

/**
 * @fn int IMP_Encoder_GetJpegeQl(int encChn, IMPEncoderJpegeQl *pstJpegeQl)
 *
 * 获取 JPEG 协议编码通道的用户量化表配置参数
 *
 * @param[in] encChn 编码Channel号, 取值范围: [0, @ref NR_MAX_ENC_CHN - 1]
 * @param[out] pstJpegeQl 返回JPEG 协议编码通道的量化表配置参数
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 如果通道未创建, 则返回失败
 * @remark 此API只适用于JPEG
 *
 * @attention 无
 */
int IMP_Encoder_GetJpegeQl(int encChn, IMPEncoderJpegeQl *pstJpegeQl);

/**
 * @brief int IMP_Encoder_SetPool(int chnNum, int poolID);
 *
 * 绑定chnnel 到内存池中，即Encoder申请mem从pool申请.
 *
 * @param[in] chnNum		通道编号.
 * @param[in] poolID		内存池编号.
 *
 * @retval 0				成功.
 * @retval 非0				失败.
 *
 * @remarks	  为了解决rmem碎片化，将该channel Encoder绑定到对应的mempool
 * 中, Encoder 申请mem就在mempool中申请，若不调用，Encoder会在rmem中申请
 * 此时对于rmem来说会存在碎片的可能
 *
 * @attention ChannelId 必须大于等于0 且小于32
 */
int IMP_Encoder_SetPool(int chnNum, int poolID);

/**
 * @brief IMP_Encoder_GetPool(int chnNum);
 *
 * 通过channel ID 获取poolID.
 *
 * @param[in] chnNum       通道编号.
 *
 * @retval  >=0 && < 32    成功.
 * @retval  <0			   失败.
 *
 * @remarks   通过ChannelId 获取PoolId，客户暂时使用不到
 *
 * @attention 无.
 */
int IMP_Encoder_GetPool(int chnNum);

/**
 * @brief int IMP_Encoder_InputJpege(uint8_t *src, uint8_t *dst, int src_w, int src_h, int q,int *stream_length);
 *
 * IVPU 外部输入NV12编码JPEG
 *
 * @param[in] *src 源数据地址指针
 * @param[in] *dst 码流数据地址指针
 * @param[in] src_w 图像高
 * @param[in] src_h 图像宽
 * @param[in] q 图像质量控制<暂不支持>
 * @param[out] stream_length 码流数据长度
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 此API只适用于宽32对齐,高8对齐的NV12输入编码JPEG
 *
 * @attention 无
 */
int IMP_Encoder_InputJpege(uint8_t *src, uint8_t *dst, int src_w, int src_h, int q,int *stream_length);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IMP_ENCODER_H__ */
