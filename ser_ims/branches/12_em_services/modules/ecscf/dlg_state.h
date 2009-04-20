/**
 * \file
 * 
 * Emergency-CSCF - Dialog State Operations
 * 
 *  \author Dragos Vingarzan vingarzan -at- fokus dot fraunhofer dot de
 *  \author Alberto Diez - Changes to handle release_call 
 *  \author Ancuta Onofrei    ancuta_onofrei -at- yahoo dot com
 */
 

#ifndef E_CSCF_DLG_STATE_H
#define E_CSCF_DLG_STATE_H

#include "../../sr_module.h"
#include "mod.h"
#include "../../locking.h"
#include "../tm/dlg.h"
#include "../../lib/lost/client.h"
#include "../../lib/lost/parsing.h"
#include "../../lib/lost/pidf_loc.h"


/** Enumeration for known dialogs */
enum e_dialog_method {
	DLG_METHOD_OTHER=0,
	DLG_METHOD_INVITE=1,
	DLG_METHOD_SUBSCRIBE=2	
};

/** The last dialog type */
#define DLG_METHOD_MAX DLG_METHOD_SUBSCRIBE

/** Enumeration for dialog states */
enum e_dialog_state {
	DLG_STATE_UNKNOWN=0,
	DLG_STATE_INITIAL=1,
	DLG_STATE_EARLY=2,
	DLG_STATE_CONFIRMED=3,
	DLG_STATE_TERMINATED_ONE_SIDE=4,
	DLG_STATE_TERMINATED=5	
};

/** Enumeration for dialog directions */
enum e_dialog_direction {
	DLG_MOBILE_ORIGINATING=0,
	DLG_MOBILE_TERMINATING=1,
	DLG_MOBILE_UNKNOWN=2
};

/** Structure for S-CSCF dialogs */
typedef struct _e_dialog {
	unsigned int hash;					/**< hash for the dialog 						*/
	str call_id;						/**< call-id for the dialog 					*/
	enum e_dialog_direction direction;	/**< direction									*/
	
	str aor;							/**< Public Identity of the user				*/
			
	enum e_dialog_method method;		/**< method of the initial request enumeration	*/
	str method_str;						/**< method of the initial request string 		*/
	int first_cseq;						/**< first (initial request) CSeq				*/
	int last_cseq;						/**< last seen CSeq								*/
	enum e_dialog_state state;			/**< state of the dialog						*/
	time_t expires;						/**< expiration time for the dialog				*/
	time_t lr_session_expires;  		/**< last remembered request - session-expires header	*/
	str refresher;						/**< session refresher							*/
	unsigned char uac_supp_timer; 		/**< requester uac supports timer 				*/
	
	str event;							/**< event header - used for release			*/
	
	unsigned char is_releasing;			/**< weather this dialog is already being 
											released or not, or its peer, with count on 
											tries 										*/	
	dlg_t *dialog_c;					/**< dialog in direction to callee           	*/
	dlg_t *dialog_s;					/**< dialog in direction to caller 				*/
	/*flag for anonymous call*/
	int anonymous;
	/*location of the client that initiated an E-call*/
	xmlNode * location;
	/*used to store the location str when an INVITE is received, do not use it otherwise it might not be correctly set*/
	str location_str;	
	loc_fmt d_loc_fmt;
	/*psap uri information*/
	str psap_uri;
	str esqk;
	uri_type psap_uri_type; /* uri scheme of the PSAP URI*/
	expire_type exp_type; /*if >=1 the mapping never expires*/
	time_t exp_timestamp;

	struct _e_dialog *next;				/**< next dialog in this dialog hash slot 		*/
	struct _e_dialog *prev;				/**< previous dialog in this dialog hash slot	*/
} e_dialog;

/** Structure for a S-CSCF dialog hash slot */
typedef struct {
	e_dialog *head;						/**< first dialog in this dialog hash slot 		*/
	e_dialog *tail;						/**< last dialog in this dialog hash slot 		*/
	gen_lock_t *lock;					/**< slot lock 									*/	
} e_dialog_hash_slot;


inline unsigned int get_e_dialog_hash(str call_id);

int e_dialogs_init(int hash_size);

void e_dialogs_destroy();

inline void d_lock(unsigned int hash);
inline void d_unlock(unsigned int hash);


e_dialog* new_e_dialog(str call_id,str aor,enum e_dialog_direction dir);
e_dialog* add_e_dialog(str call_id,str aor,enum e_dialog_direction dir);
int is_e_dialog(str call_id,str aor,enum e_dialog_direction dir);
int is_e_dialog_dir(str call_id,enum e_dialog_direction dir);
e_dialog* get_e_dialog(str call_id,str aor);
e_dialog* get_e_dialog_dir(str call_id,enum e_dialog_direction dir);
e_dialog* get_e_dialog_dir_nolock(str call_id,enum e_dialog_direction dir);
int terminate_e_dialog(e_dialog *d);
void del_e_dialog(e_dialog *d);
void free_e_dialog(e_dialog *d);
void print_e_dialogs(int log_level);
enum e_dialog_direction get_dialog_direction(char *direction);


int E_is_in_dialog(struct sip_msg* msg, char* str1, char* str2);

int E_save_dialog(struct sip_msg* msg, char* str1, char* str2);

int E_update_dialog(struct sip_msg* msg, char* str1, char* str2);

int E_drop_dialog(struct sip_msg* msg, char* str1, char* str2);

int E_drop_all_dialogs(str aor);

void dialog_timer(unsigned int ticks, void* param);

int E_check_session_expires(struct sip_msg* msg, char* str1, char* str2);

int E_422_session_expires(struct sip_msg* msg, char* str1, char* str2);
		
#endif