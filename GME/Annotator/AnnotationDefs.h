#ifndef ANNOTATION_DEFS_H
#define ANNOTATION_DEFS_H

#define E_ANNOTATOR_INVALID_PARAMVALUE	0x80737051

#define AN_DECORATOR_PROGID			"MGA.Decorator.Annotator"

#define AN_PARAM_ROOTNODE			_T("rootnode")

#define AN_ROOT						_T("annotations")
#define AN_INHERITABLE				_T("inheritable")
#define AN_HIDDEN					_T("hidden")
#define AN_COLOR_PREF				_T("color")
#define AN_BGCOLOR_PREF				_T("bgcolor")
#define AN_GRADIENTCOLOR_PREF		_T("gradientcolor")
#define AN_SHADOWCOLOR_PREF			_T("shadowcolor")
#define AN_FONT_PREF				_T("logfont")
#define AN_ASPECTS					_T("aspects")
#define AN_DEFASPECT				_T("*")
#define AN_GRADIENTFILL_PREF		_T("gradientfill")
#define AN_GRADIENTDIRECTION_PREF	_T("gradientdirection")
#define AN_CASTSHADOW_PREF			_T("castshadow")
#define AN_SHADOWDEPTH_PREF			_T("shadowdepth")
#define AN_SHADOWDIRECTION_PREF		_T("shadowdirection")
#define AN_ROUNDCORNERRECT_PREF		_T("roundrectcorner")
#define AN_ROUNDCORNERRADIUS_PREF	_T("roundrectcornerradius")

#define	AN_VISIBLE_DEFAULT			_T("DefaultVisibility")
#define AN_DEFANNOTATION_NAME		_T("Annotation")
#define AN_DEFAULTANNOTATION_TXT	_T("Enter your annotation here")

#define AN_DEFAULT_FONT_HEIGHT			12
#define AN_DEFAULT_FONT_FACE			_T("Arial")
#define	AN_DEFAULT_COLOR				RGB(0x00, 0x00, 0x00)
#define AN_COLOR_TRANSPARENT			-1
#define	AN_DEFAULT_BGCOLOR				AN_COLOR_TRANSPARENT
#define	AN_DEFAULT_SHADOWCOLOR			RGB(0xC0, 0xC0, 0xC0)
#define	AN_DEFAULT_GRAYEDOUTCOLOR		RGB(0xde, 0xde, 0xde)
#define	AN_DEFAULT_GRADIENTCOLOR		AN_DEFAULT_COLOR
#define AN_DEFAULT_GRADIENTFILL			false
#define AN_DEFAULT_GRADIENTDIRECTION	0
#define AN_DEFAULT_CASTSHADOW			false
#define AN_DEFAULT_SHADOWDEPTH			9
#define AN_DEFAULT_SHADOWDIRECTION		45
#define AN_DEFAULT_ROUNDCORNERRECT		false
#define AN_DEFAULT_ROUNDCORNERRADIUS	9

#define AN_MARGIN						6

#endif	//ANNOTATION_DEFS_H