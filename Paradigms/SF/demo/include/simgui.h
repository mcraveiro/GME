/**
 ** SIMGUI.H
 **
 ** Interface declarations for the simulation user interface.
 ** These are prototypes of the API functions for C or C++ applications.
 **/
#ifndef __SIMGUI_H_INCLUDED__
#define __SIMGUI_H_INCLUDED__

#ifdef  __cplusplus
extern  "C" {
#endif

/* startup */
void	gui_start(void);
void	gui_set_config_file(const char *fname);
void	gui_set_init_file(const char *fname);

/* C <=> Fortran string conversions */
void	gui_strtrim(char *cstr,const char *fstr,int len);
void	gui_strfill(char *fstr,const char *cstr,int len);

/* register functions into the UI, and other pre-startup stuff */
typedef void  (*gui_runfn)(int);
typedef void  (*gui_cleanupfn)(void);
typedef void  (*gui_scanfn)(int);
typedef void  (*gui_plotfn)(void);
typedef void  (*gui_convfn)(int n,float *out,float *in1,float *in2,float *in3);

void	gui_register_run_entry(gui_runfn fn,const char *name);
void	gui_register_runmenu_entry(gui_runfn fn,const char *name);
void	gui_register_run_background(gui_runfn fn,const char *name);
void	gui_register_run_cleanup(gui_cleanupfn fn,const char *name);
void	gui_set_autorun(const char *name,int forcedlgs);
void	gui_register_scanner(gui_scanfn fn,const char *name);
void	gui_register_datamenu_entry(gui_scanfn fn,const char *name);
void	gui_register_function(gui_convfn fn,int nargs,const char *name);
void	gui_register_plot(gui_plotfn fn,const char *name);
void	gui_register_commandplot(gui_plotfn fn,const char *name);
void	gui_debug_to_console(void);
void	gui_register_help_file(const char *title,const char *fname);
void	gui_register_help_text(const char *title,const char *htext,int txtln);
#ifdef  _MSC_VER
void	gui_register_winhelp_file(const char *title,const char *fname);
#endif
void	gui_set_protection_mode(int mode);

/* if a registered name starts with this, don't put it into a menu */
#define GUI_NO_MENU_APPEND		"## - nomenu - ##"

/* protection modes for 'gui_set_protection_mode' */
#define GUI_DEFAULT_PROTECTION		0
#define GUI_DEVELOPER_PROTECTION	1
#define GUI_END_USER_PROTECTION		2

/* data base interface */
void	gui_setup_database(const char *dbname,const char *xvname);
void	gui_clear_data(const char *name,const char *dbname);
void	gui_erase_data(const char *name,const char *dbname,int start,int count);
void	gui_insert_data(const char *name,const char *dbname,int start,const float *data,int count);
void	gui_replace_data(const char *name,const char *dbname,int start,const float *data,int count);
void	gui_append_data(const char *name,const char *dbname,const float *data,int count);
void	gui_read_data(const char *name,const char *dbname,int start,float *data,int count);
int	gui_data_size(const char *name,const char *dbname);
void	gui_clear_database(const char *dbname);
void	gui_erase_database(const char *dbname,int pts_to_remove);
void	gui_inherit_plots(const char *DBfrom,const char *DBto);
void	gui_remove_plots(const char *DBname);
void	gui_set_database_limits(const char *dbname,int maxlen,int maxplt);
void	gui_set_var_description(const char *name,const char *desc);
void	gui_set_dbase_description(const char *dbname,const char *desc);
int	gui_database_length_limit(const char *dbname);
int	gui_database_plot_limit(const char *dbname);
const	char *gui_var_description(const char *name);
const	char *gui_dbase_description(const char *name);
const	float *gui_get_data(const char *name,const char *dbname);

/* utilities callable by registered "run" functions */
void	gui_update(void);
void	gui_update_plots(void);
void	gui_update_selected_plots(int dodata,int dosubr);
void	gui_plot_changed(const char *name);
void	gui_add_plot_data(const char *name,const float *data,int items);
void	gui_get_run_classification(char *name);
void	gui_set_run_classification(const char *name);
void	gui_runerror(const char *msg,...);
void	gui_runmessage(const char *msg,...);
void	gui_abort_run(void);
void	gui_stop_background(void);
void	gui_wait_for_event(int tout_msec);

/* run-time controls */
void	gui_define_control(const char *name,double minval,double maxval,double initval);
double  gui_get_control_value(const char *name);
void	gui_set_control_value(const char *name,double value);
void	gui_set_control_limits(const char *name,double minval,double maxval);
void	gui_define_icontrol(const char *name,int minval,int maxval,int initval);
int	gui_get_icontrol_value(const char *name);
void	gui_set_icontrol_value(const char *name,int value);
void	gui_set_icontrol_limits(const char *name,int minval,int maxval);
void	gui_define_adjuster(const char *name,double add,double mult,double init);
double  gui_get_adjuster_value(const char *name);
void	gui_set_adjuster_value(const char *name,double value);
void	gui_set_adjuster_params(const char *name,double add,double mult);
void	gui_define_iadjuster(const char *name,int add,int mult,int init);
int	gui_get_iadjuster_value(const char *name);
void	gui_set_iadjuster_value(const char *name,int value);
void	gui_set_iadjuster_params(const char *name,int add,int mult);
void	gui_define_radio(const char *name,const char *buttons);
int	gui_get_radio_value(const char *name);
void	gui_set_radio_value(const char *name,int which);
void	gui_define_label(const char *name);
const	char *gui_get_label_value(const char *name);
void	gui_set_label_value(const char *name,char *text);
void	gui_define_button(const char *name);
int	gui_get_button_value(const char *name);
void	gui_set_button_value(const char *name,int depress);
void	gui_define_toggle(const char *name);
int	gui_get_toggle_value(const char *name);
void	gui_set_toggle_value(const char *name,int depress);
int	gui_show_control_group(const char *grpname,int visible);

#define GUI_RADIO_SEPARATOR	"/-.-/"
#define GUI_CONTROL_RADIO_SEPAR GUI_RADIO_SEPARATOR
#define GUI_CONTROL_GROUP_SEPAR "::.::"

/* utilities for programmed plots */
void	plot_open_curve_2d(void);
void	plot_add_curve_points_2d(int count,const float *xdata,const float *ydata);
void	plot_add_curve_separator_2d(void);
void	plot_set_curve_title_2d(const char *title);
void	plot_set_curve_style_2d(int style);
void	plot_set_curve_line_type_2d(int type);
void	plot_set_curve_point_type_2d(int type);
void	plot_close_curve_2d(void);
int	plot_window_index(void);
char   *gui_get_plot_info(void);
void	gui_set_plot_info(const char *info);
void	gui_plot_command(const char *cmd);
void    gui_plot_3d(const char *cmd,const char *key,int nx,int ny,int offset,float *x,float *y, float *z);
void    gui_plot_3d_p(const char *cmd,const char *key,int nx,int ny,float *x,float *y, float **z);
int	gui_get_current_page(void);
void	gui_set_current_page(int pg);

/* plot related constants: plot styles */
#define PLOT_STYLE_DEFAULT	0
#define PLOT_STYLE_LINES	1
#define PLOT_STYLE_POINTS	2
#define PLOT_STYLE_IMPULSES	3
#define PLOT_STYLE_LINESPNTS	4
#define PLOT_STYLE_DOTS		5
#define PLOT_STYLE_ERRORBARS	6
#define PLOT_STYLE_BOXES	7
#define PLOT_STYLE_BOXERROR	8
#define PLOT_STYLE_STEPS	9

/* point types */
#define PLOT_PTYPE_DEFAULT	(-1)
#define PLOT_PTYPE_DIAMOND	0
#define PLOT_PTYPE_PLUS		1
#define PLOT_PTYPE_BOX		2
#define PLOT_PTYPE_CROSS	3
#define PLOT_PTYPE_TRIANGLE	4
#define PLOT_PTYPE_STAR		5

/* line colors (= GNUPLOT line type) colors valid only on UI screen */
#define PLOT_LTYPE_BORDER	(-2)
#define PLOT_LTYPE_GRID		(-1)
#define PLOT_LTYPE_DEFAULT	(-1)
#define PLOT_LTYPE_GREEN	0
#define PLOT_LTYPE_CYAN		1
#define PLOT_LTYPE_RED		2
#define PLOT_LTYPE_MAGENTA	3
#define PLOT_LTYPE_YELLOW	4
#define PLOT_LTYPE_ORANGE	5
#define PLOT_LTYPE_DGREEN	6
#define PLOT_LTYPE_DCYAN	7
#define PLOT_LTYPE_DRED		8
#define PLOT_LTYPE_DMAGENTA	9
#define PLOT_LTYPE_DYELLOW	10
#define PLOT_LTYPE_DORANGE	11

/* low-level plot interface */
typedef struct {
    int	   maxx,maxy;
    int	   fntw,fnth;
    int	   xtic,ytic;
    void (*move)(int x,int y);
    void (*line)(int x,int y);
    void (*linetype)(int ltype);
    void (*text)(int x,int y,char *txt);
    int  (*textangle)(int ang);
    int  (*textjustify)(int just);
} gui_plot_win_info;

int  gui_begin_plot(gui_plot_win_info *info);
void gui_finish_plot(void);

/* low-level plot window interface constants */
#define GUI_PLOT_TEXT_ANGLE_HORIZONTAL  0
#define GUI_PLOT_TEXT_ANGLE_VERTICAL	1
#define GUI_PLOT_TEXT_JUSTIFY_LEFT	0
#define GUI_PLOT_TEXT_JUSTIFY_CENTER	1
#define GUI_PLOT_TEXT_JUSTIFY_RIGHT	2

/* inform, ask */
typedef struct {
    const char *prompt;
    char       *reply;
    int		maxrsize;
} gui_askentry;

int  gui_ask(
    int	  force,
    const char *title,
    const char *prompt,char *reply,int maxrsize,
    ...
);

int  gui_askn(int force,const char *title,int n,gui_askentry *fields);
void gui_inform(const char *msg,...);
int  gui_confirm_yes_no(const char *msg,...);
int  gui_confirm_ok_cancel(const char *msg,...);
int  gui_confirm_yes_no_cancel(const char *msg,...);
void gui_message(const char *msg,...);
void gui_error(const char *msg,...);
void gui_beep(void);

/* prompt string headers for different dialog types */
#define GUI_ASK_LONGSTR		"LS"		/* long string (80 char box) */
#define GUI_ASK_SHORTSTR	"SS"		/* short string (40 chars) */
#define GUI_ASK_VERYSHORTSTR	"VS"		/* very short string (10 ch) */
#define GUI_ASK_FILENAME	"FN"		/* file name */
#define GUI_ASK_INTEGER		"IN"		/* integer number */
#define GUI_ASK_FLOAT		"FL"		/* floating point number */
#define GUI_ASK_YESNO		"YN"		/* yes/no (answer: "Y"/"N") */
#define GUI_ASK_RADIO		"RD"		/* select via radio buttons */
#define GUI_ASK_RADIO_H		"HR"		/* horizontal radio options */
#define GUI_ASK_DATASET		"DS"		/* pick a data set name */
#define GUI_ASK_VARIABLE	"VR"		/* pick a variable */
#define GUI_ASK_SET_AND_VAR	"SV"		/* pick a data set and a var */

/* reply string headers to change default behavior */
#define GUI_ASK_NOLOOKUP	"@@"		/* don't look up history */

/* radio prompt/button name separators */
#define GUI_ASK_RADIO_SEPAR	GUI_RADIO_SEPARATOR

/* curve set editor utility */
typedef struct {
    int		npoints;
    int		maxpnts;
    float      *xpoints;
    float      *ypoints;
    char       *title;		/* name of this curve */
    float	special;	/* an optional per curve special value */
    int		nattrbs;	/* # of per-point attributes */
    char      **prompts;	/* prompts to edit per-point attributes */
    char     ***attribs;	/* char *attribs[nattrbs][npoints] */
} gui_curve;

typedef struct {
    int		ncurves;
    int		maxcrvs;
    gui_curve **curves;
} gui_curveset;

typedef gui_curve *(*gui_curve_genfn)(gui_curve *incurve);

typedef int (*gui_curveset_verifyfn)(
    gui_curveset *cset,
    char *title,
    char *msgbuf
);
typedef int (*gui_multi_cset_verifyfn)(
    int n,
    gui_curveset *csets[],
    char *titles[],
    char *msgbuf
);

gui_curve    *gui_alloc_curve(int maxpoints,char *crvtitle);
gui_curveset *gui_alloc_curveset(int maxcurves);

void  gui_set_attribute(gui_curve *crv,int pt,int attr,const char *attrval);
char *gui_attribute_value(gui_curve *crv,int pt,int attr);

void  gui_free_curve(gui_curve *crv);
void  gui_free_curveset(gui_curveset *cset);

gui_curveset *gui_edit_curveset(
    char		    *title,
    char		    *specname,
    gui_curveset	    *origset,
    char		    *ptitle,
    char		    *pxlabel,
    char		    *pylabel,
    gui_curve_genfn	    curvefitgen,
    gui_curve_genfn	    origfitgen,
    gui_curveset_verifyfn   verify
);

int gui_edit_multi_cset(
    char		    *title,
    char		    *specname,
    int			    nsets,
    gui_curveset	    *origsets[],
    char		    *ptitles[],
    char		    *pxlabels[],
    char		    *pylabels[],
    gui_curve_genfn	    curvefitgens[],
    gui_curve_genfn	    origfitgens[],
    gui_multi_cset_verifyfn verify,
    gui_curveset	    *outsets[]
);

/* ITIS interface */
void gui_register_itis(void);
void gui_set_itis_host(char *host,char *port);

#ifdef  __cplusplus
}
#endif

#endif  /* whole file */

