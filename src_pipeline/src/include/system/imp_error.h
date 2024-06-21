#ifndef __IMP_ERROR_H__
#define __IMP_ERROR_H__

#define IMP_WARN_RETURN_NULL(tag, code, fmt, ...) {\
	printf("WARNING: [%s & %d]  CODE=%d", __func__, __LINE__, code); \
	printf(fmt, ##__VA_ARGS__);}

#define IMP_ERROR_RETURN_NULL(tag, code, fmt, ...) {\
	printf("ERR: [%s & %d]  CODE=%d", __func__, __LINE__, code); \
	printf(fmt, ##__VA_ARGS__); \
	return NULL;}

#define IMP_WARN_RETURN(tag, code, fmt, ...) {\
	printf("WARNING: [%s & %d]  CODE=%d", __func__, __LINE__, code); \
	printf(fmt, ##__VA_ARGS__);}

#define IMP_ERROR_RETURN(tag, code, fmt, ...) {\
	printf("ERR: [%s & %d]  CODE=%d", __func__, __LINE__, code); \
	printf(fmt, ##__VA_ARGS__); \
	return -code;}

#define IMP_ERR_BASE            0x1000
#define EIOCTLCMD		IMP_ERR_BASE+1 		/* ioctl command error */
#define EMEMALLO		IMP_ERR_BASE+2 		/* Memory allocation error */
#define EVERMISMATH		IMP_ERR_BASE+3 		/* Version mismatch */
#define EVALOVERFLOW	        IMP_ERR_BASE+4 		/* value is overflow */
#define EISPDEVOPS		IMP_ERR_BASE+5 		/* ispdev ops error */
#define EISPTUNCMD 		IMP_ERR_BASE+6 		/* isp image tuning private cmd error */
#define EINVALIDFUN		IMP_ERR_BASE+7 		/* Invalid function */
#define EOPEN			IMP_ERR_BASE+8 		/* Failed to open */
#define ESTATE			IMP_ERR_BASE+9 		/* State error */
#define	ENOTSUPP		IMP_ERR_BASE+10		/* Cannot support this operation */
#define ESVIC			IMP_ERR_BASE+11		/* Note the status of the VIC */
#define ESNUMOVERFLOW           IMP_ERR_BASE+12         /* Number of sensors overflow */

#endif
