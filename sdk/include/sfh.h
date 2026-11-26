/* the Sofdec stream header's reader. a query answers once SFH_IsSfdHeader has
 * accepted the handle's header. each returns TRUE where it read its value into
 * its last argument. a text query writes its field and a NUL to out. a name or
 * a time stamp takes 13 bytes, the tool 33 and the comment 65 */
#ifndef SFH_H
#define SFH_H
#include <sg_xpt.h>
struct sfh_header;
struct sfh_handle {
    Sint32 status;
    const struct sfh_header *header;
    Uint32 size;
    Sint32 version;
};
/* a header's tool version, major.minor, as one number, as struct
 * sfh_handle's version keeps it */
#define SFH_VERSION(major, minor) (((major) * 100) + (minor))
/* the tool version from which a header gives its byte rate and its streams'
 * features */
#define SFH_FEATURE_VERSION SFH_VERSION(1, 10)

const char *SFH_GetSbverStr(void);
Bool SFH_AnlySfdFname(struct sfh_handle *handle, char *out);
Bool SFH_AnlySfdTimStmp(struct sfh_handle *handle, char *out);
Bool SFH_AnlyHdrToolInf(struct sfh_handle *handle, char *out);
Bool SFH_AnalyHdrComment(struct sfh_handle *handle, char *out);
Bool SFH_AnlyElemFname(struct sfh_handle *handle, Uint8 stream, char *out);
Bool SFH_AnlyElemTimStmp(struct sfh_handle *handle, Uint8 stream, char *out);
void SFH_Init(Sint32 count, struct sfh_handle *handles);
void SFH_Finish(void);
struct sfh_handle *SFH_Create(const void *data, Sint32 size);
void SFH_Destroy(struct sfh_handle *handle);
Bool SFH_IsSfdHeader(struct sfh_handle *handle, Bool *is_sfd);
Bool SFH_IsExistStmId(struct sfh_handle *handle, Uint8 stream, Bool *exists);
Bool SFH_IsEffFtrInf(struct sfh_handle *handle, Uint8 stream, Bool *available);
Bool SFH_AnlyHdrToolVer(struct sfh_handle *handle, Sint32 *major,
                        Sint32 *minor);
Bool SFH_AnlyElemPicSz(struct sfh_handle *handle, Uint8 stream, Sint32 *width,
                       Sint32 *height);
typedef Bool sfh_query(struct sfh_handle *handle, Sint32 *value);
typedef Bool sfh_stream_query(struct sfh_handle *handle, Uint8 stream,
                              Sint32 *value);
sfh_query SFH_AnlyHdrSiz, SFH_AnlyPackType, SFH_AnlyPketSizLen, SFH_AnlyPackSiz;
sfh_query SFH_AnlyNumElemTot, SFH_AnlyNumElemAud, SFH_AnlyNumElemVid,
    SFH_AnlyNumElemPrv;
sfh_query SFH_AnlyByteRate, SFH_AnlyMaxPlyLenAud, SFH_AnlyMaxPlyLenVid,
    SFH_AnlyMaxFrmNum;
sfh_stream_query SFH_AnlyElemCodecAud, SFH_AnlyElemLayer, SFH_AnlyElemChNum,
    SFH_AnlyElemSmpHz;
sfh_stream_query SFH_AnlyElemCodecVid, SFH_AnlyElemBitRate, SFH_AnlyElemPicRate;
sfh_stream_query SFH_AnlyFtrColType, SFH_AnlyFtrPicType, SFH_AnlyFtrFixFlg,
    SFH_AnlyFtrShcFixFlg;
sfh_stream_query SFH_AnlyFtrExpand, SFH_AnlyFtrGopN, SFH_AnlyFtrGopM;
#endif
