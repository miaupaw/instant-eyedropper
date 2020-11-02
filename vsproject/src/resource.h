//{{NO_DEPENDENCIES}}
// Handmade
// Used by instanteyedropper.rc

///////////////////////////////////////////////////////////////////////////////////////////////////
// version
#define version_major   2
#define version_minor   0
#define version_build   0
#define version_revis   0

#define STRINGIFY(s) #s
#define TOSTRING(s) STRINGIFY(s)

// synthesis of variables
#define vsfile          version_major, version_minor, version_build, version_revis
#define vsproduct       version_major, version_minor, version_build

#define vsfilestr       TOSTRING(version_major)    "." \
                        TOSTRING(version_minor)    "." \
                        TOSTRING(version_build)    "." \
                        TOSTRING(version_revis)

#define vsproductstr    TOSTRING(version_major)    "." \
                        TOSTRING(version_minor)    "." \
                        TOSTRING(version_build)

///////////////////////////////////////////////////////////////////////////////////////////////////
// icon
#define IDI_ICON1                       2200    // icon

// bitmaps
#define IDB_EYEDROPPER                  3201    // eyedropper window common
#define IDB_EYEDROPPERDARK              3202    // eyedropper window dark
#define IDB_ABOUT                       3203    // about.bmp

// dialog window, options + about
#define IDD_DIALOG                      4000    // dialog window
#define  IDC_TAB                        4001    // tab control

#define IDCG_GENERAL                    5001    // groupbox: general
#define  IDCK_STARTUP                   5002    // startup
#define  IDCK_THEME                     5003    // theme
#define  IDCK_CPTOCPB                   5004    // copy to clipboard

#define IDCG_HOTKEY                     5101    // groupbox: hotkey
#define  IDCB_MOD0                      5102    // mod
#define  IDCB_MOD1                      5103    // mod
#define  IDCB_MOD2                      5104    // mod
#define  IDCT_PLUS                      5105    // +
#define  IDCB_VKEY                      5106    // vkey

#define IDCG_FORMAT                     5201    // groupbox: color format
#define  IDCB_FORMAT                    5202    // combo: color formats
#define   IDCT_PRCN                     5320    // static: precision:
#define   IDCB_PRCN                     5321    // combo: precision
#define   IDCT_TEMPLATE                 5340    // static: template:
#define   IDCB_TEMPLATE                 5341    // combo: template

#define IDCG_MODE                       5401    // groupbox: mode
#define  IDCR_DRAG                      5402    // drag mode
#define  IDCR_CLICK                     5403    // click mode

#define IDCG_ZOOM                       5501    // groupbox: resolution
#define  IDCB_ZOOM                      5502    // combo: resolution

#define IDCO_ABOUT                      6001    // static: background pic
#define  IDCT_VERSION                   6002    // static: version number
#define  IDCT_HOME                      6003    // static: home
#define  IDCT_COPYRIGHT                 6004    // static: copyright
#define  IDCT_IEYEDROPPERURL            6005    // static: instant-eyedropper.com
#define  IDCT_AUTHOR                    6006    // static: me
#define  IDB_UPDATE                     6007    // update button

#define ID_CONTROLLER                   8000    // для приема сообщений от главного контроллера

///////////////////////////////////////////////////////////////////////////////////////////////////
