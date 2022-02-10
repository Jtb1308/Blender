/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2008 Blender Foundation.
 * All rights reserved.
 */

/** \file
 * \ingroup editors
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct AnimData;
struct Depsgraph;
struct ID;
struct ListBase;

struct ARegion;
struct ARegionType;
struct FModifier;
struct Main;
struct NlaStrip;
struct PanelType;
struct ReportList;
struct ScrArea;
struct SpaceLink;
struct View2D;
struct bContext;
struct wmKeyConfig;

struct Object;
struct Scene;

struct bDopeSheet;

struct FCurve;
struct FModifier;
struct bAction;

struct uiBlock;

struct PointerRNA;
struct PropertyRNA;

/* ************************************************ */
/* ANIMATION CHANNEL FILTERING */
/* anim_filter.c */

/* -------------------------------------------------------------------- */
/** \name Context
 * \{ */

/* This struct defines a structure used for animation-specific
 * 'context' information
 */
typedef struct bAnimContext {
  /** data to be filtered for use in animation editor */
  void *data;
  /** type of data eAnimCont_Types */
  short datatype;

  /** editor->mode */
  short mode;
  /** area->spacetype */
  short spacetype;
  /** active region -> type (channels or main) */
  short regiontype;

  /** editor host */
  struct ScrArea *area;
  /** editor data */
  struct SpaceLink *sl;
  /** region within editor */
  struct ARegion *region;

  /** dopesheet data for editor (or which is being used) */
  struct bDopeSheet *ads;

  /** Current Main */
  struct Main *bmain;
  /** active scene */
  struct Scene *scene;
  /** active scene layer */
  struct ViewLayer *view_layer;
  /** active dependency graph */
  struct Depsgraph *depsgraph;
  /** active object */
  struct Object *obact;
  /** active set of markers */
  ListBase *markers;

  /** pointer to current reports list */
  struct ReportList *reports;

  /** Scale factor for height of channels (i.e. based on the size of keyframes). */
  float yscale_fac;
} bAnimContext;

/* Main Data container types */
typedef enum eAnimCont_Types {
  ANIMCONT_NONE = 0,      /* invalid or no data */
  ANIMCONT_ACTION = 1,    /* action (bAction) */
  ANIMCONT_SHAPEKEY = 2,  /* shapekey (Key) */
  ANIMCONT_GPENCIL = 3,   /* grease pencil (screen) */
  ANIMCONT_DOPESHEET = 4, /* dopesheet (bDopesheet) */
  ANIMCONT_FCURVES = 5,   /* animation F-Curves (bDopesheet) */
  ANIMCONT_DRIVERS = 6,   /* drivers (bDopesheet) */
  ANIMCONT_NLA = 7,       /* nla (bDopesheet) */
  ANIMCONT_CHANNEL = 8,   /* animation channel (bAnimListElem) */
  ANIMCONT_MASK = 9,      /* mask dopesheet */
  ANIMCONT_TIMELINE = 10, /* "timeline" editor (bDopeSheet) */
} eAnimCont_Types;

/** \} */

/* -------------------------------------------------------------------- */
/** \name Channels
 * \{ */

/* This struct defines a structure used for quick and uniform access for
 * channels of animation data
 */
typedef struct bAnimListElem {
  struct bAnimListElem *next, *prev;

  /** source data this elem represents */
  void *data;
  /** (eAnim_ChannelType) one of the ANIMTYPE_* values */
  int type;
  /** copy of elem's flags for quick access */
  int flag;
  /** for un-named data, the index of the data in its collection */
  int index;

  /** (eAnim_Update_Flags)  tag the element for updating */
  char update;
  /** tag the included data. Temporary always */
  char tag;

  /** (eAnim_KeyType) type of motion data to expect */
  short datatype;
  /** motion data - mostly F-Curves, but can be other types too */
  void *key_data;

  /**
   * \note
   * id here is the "IdAdtTemplate"-style datablock (e.g. Object, Material, Texture, NodeTree)
   * from which evaluation of the RNA-paths takes place. It's used to figure out how deep
   * channels should be nested (e.g. for Textures/NodeTrees) in the tree, and allows property
   * lookups (e.g. for sliders and for inserting keyframes) to work. If we had instead used
   * bAction or something similar, none of this would be possible: although it's trivial
   * to use an IdAdtTemplate type to find the source action a channel (e.g. F-Curve) comes from
   * (i.e. in the AnimEditors, it *must* be the active action, as only that can be edited),
   * it's impossible to go the other way (i.e. one action may be used in multiple places).
   */
  /** ID block that channel is attached to */
  struct ID *id;
  /** source of the animation data attached to ID block (for convenience) */
  struct AnimData *adt;

  /**
   * For list element which corresponds to a f-curve, this is an ID which
   * owns the f-curve.
   *
   * For example, if the f-curve is coming from Action, this id will be set to
   * action's ID. But if this is a f-curve which is a driver, then the owner
   * is set to, for example, object.
   *
   * NOTE: this is different from id above. The id above will be set to
   * an object if the f-curve is coming from action associated with that object.
   */
  struct ID *fcurve_owner_id;

  /**
   * for per-element F-Curves
   * (e.g. NLA Control Curves), the element that this represents (e.g. NlaStrip) */
  void *owner;
} bAnimListElem;

/**
 * Some types for easier type-testing
 *
 * \note need to keep the order of these synchronized with the channels define code
 * which is used for drawing and handling channel lists for.
 */
typedef enum eAnim_ChannelType {
  ANIMTYPE_NONE = 0,
  ANIMTYPE_ANIMDATA,
  ANIMTYPE_SPECIALDATA__UNUSED,

  ANIMTYPE_SUMMARY,

  ANIMTYPE_SCENE,
  ANIMTYPE_OBJECT,
  ANIMTYPE_GROUP,
  ANIMTYPE_FCURVE,

  ANIMTYPE_NLACONTROLS,
  ANIMTYPE_NLACURVE,

  ANIMTYPE_FILLACTD,
  ANIMTYPE_FILLDRIVERS,

  ANIMTYPE_DSMAT,
  ANIMTYPE_DSLAM,
  ANIMTYPE_DSCAM,
  ANIMTYPE_DSCACHEFILE,
  ANIMTYPE_DSCUR,
  ANIMTYPE_DSSKEY,
  ANIMTYPE_DSWOR,
  ANIMTYPE_DSNTREE,
  ANIMTYPE_DSPART,
  ANIMTYPE_DSMBALL,
  ANIMTYPE_DSARM,
  ANIMTYPE_DSMESH,
  ANIMTYPE_DSTEX,
  ANIMTYPE_DSLAT,
  ANIMTYPE_DSLINESTYLE,
  ANIMTYPE_DSSPK,
  ANIMTYPE_DSGPENCIL,
  ANIMTYPE_DSMCLIP,
  ANIMTYPE_DSHAIR,
  ANIMTYPE_DSPOINTCLOUD,
  ANIMTYPE_DSVOLUME,
  ANIMTYPE_DSSIMULATION,

  ANIMTYPE_SHAPEKEY,

  ANIMTYPE_GPDATABLOCK,
  ANIMTYPE_GPLAYER,

  ANIMTYPE_MASKDATABLOCK,
  ANIMTYPE_MASKLAYER,

  ANIMTYPE_NLATRACK,
  ANIMTYPE_NLAACTION,

  ANIMTYPE_PALETTE,

  /* always as last item, the total number of channel types... */
  ANIMTYPE_NUM_TYPES,
} eAnim_ChannelType;

/* types of keyframe data in bAnimListElem */
typedef enum eAnim_KeyType {
  ALE_NONE = 0, /* no keyframe data */
  ALE_FCURVE,   /* F-Curve */
  ALE_GPFRAME,  /* Grease Pencil Frames */
  ALE_MASKLAY,  /* Mask */
  ALE_NLASTRIP, /* NLA Strips */

  ALE_ALL,   /* All channels summary */
  ALE_SCE,   /* Scene summary */
  ALE_OB,    /* Object summary */
  ALE_ACT,   /* Action summary */
  ALE_GROUP, /* Action Group summary */
} eAnim_KeyType;

/* Flags for specifying the types of updates (i.e. recalculation/refreshing) that
 * needs to be performed to the data contained in a channel following editing.
 * For use with ANIM_animdata_update()
 */
typedef enum eAnim_Update_Flags {
  ANIM_UPDATE_DEPS = (1 << 0),    /* referenced data and dependencies get refreshed */
  ANIM_UPDATE_ORDER = (1 << 1),   /* keyframes need to be sorted */
  ANIM_UPDATE_HANDLES = (1 << 2), /* recalculate handles */
} eAnim_Update_Flags;

/* used for most tools which change keyframes (flushed by ANIM_animdata_update) */
#define ANIM_UPDATE_DEFAULT (ANIM_UPDATE_DEPS | ANIM_UPDATE_ORDER | ANIM_UPDATE_HANDLES)
#define ANIM_UPDATE_DEFAULT_NOHANDLES (ANIM_UPDATE_DEFAULT & ~ANIM_UPDATE_HANDLES)

/** \} */

/* -------------------------------------------------------------------- */
/** \name Filtering
 * \{ */

/* filtering flags  - under what circumstances should a channel be returned */
typedef enum eAnimFilter_Flags {
  /**
   * Data which channel represents is fits the dope-sheet filters
   * (i.e. scene visibility criteria).
   *
   * XXX: it's hard to think of any examples where this *ISN'T* the case...
   * perhaps becomes implicit?
   */
  ANIMFILTER_DATA_VISIBLE = (1 << 0),
  /** channel is visible within the channel-list hierarchy
   * (i.e. F-Curves within Groups in ActEdit) */
  ANIMFILTER_LIST_VISIBLE = (1 << 1),
  /** channel has specifically been tagged as visible in Graph Editor (* Graph Editor Only) */
  ANIMFILTER_CURVE_VISIBLE = (1 << 2),

  /** include summary channels and "expanders" (for drawing/mouse-selection in channel list) */
  ANIMFILTER_LIST_CHANNELS = (1 << 3),

  /** for its type, channel should be "active" one */
  ANIMFILTER_ACTIVE = (1 << 4),
  /** channel is a child of the active group (* Actions specialty) */
  ANIMFILTER_ACTGROUPED = (1 << 5),

  /** channel must be selected/not-selected, but both must not be set together */
  ANIMFILTER_SEL = (1 << 6),
  ANIMFILTER_UNSEL = (1 << 7),

  /** editability status - must be editable to be included */
  ANIMFILTER_FOREDIT = (1 << 8),
  /** only selected animchannels should be considerable as editable - mainly
   * for Graph Editor's option for keys on select curves only */
  ANIMFILTER_SELEDIT = (1 << 9),

  /**
   * Flags used to enforce certain data types.
   *
   * \note The ones for curves and NLA tracks were redundant and have been removed for now.
   */
  ANIMFILTER_ANIMDATA = (1 << 10),

  /** duplicate entries for animation data attached to multi-user blocks must not occur */
  ANIMFILTER_NODUPLIS = (1 << 11),

  /** for checking if we should keep some collapsed channel around (internal use only!) */
  ANIMFILTER_TMP_PEEK = (1 << 30),

  /** Ignore ONLYSEL flag from #bDopeSheet.filterflag (internal use only!) */
  ANIMFILTER_TMP_IGNORE_ONLYSEL = (1u << 31),
} eAnimFilter_Flags;

/** \} */

/* -------------------------------------------------------------------- */
/** \name Flag Checking Macros
 * \{ */

/* XXX check on all of these flags again. */

/* Dopesheet only */
/* 'Scene' channels */
#define SEL_SCEC(sce) (CHECK_TYPE_INLINE(sce, Scene *), ((sce->flag & SCE_DS_SELECTED)))
#define EXPANDED_SCEC(sce) (CHECK_TYPE_INLINE(sce, Scene *), ((sce->flag & SCE_DS_COLLAPSED) == 0))
/* 'Sub-Scene' channels (flags stored in Data block) */
#define FILTER_WOR_SCED(wo) (CHECK_TYPE_INLINE(wo, World *), (wo->flag & WO_DS_EXPAND))
#define FILTER_LS_SCED(linestyle) ((linestyle->flag & LS_DS_EXPAND))
/* 'Object' channels */
#define SEL_OBJC(base) (CHECK_TYPE_INLINE(base, Base *), ((base->flag & SELECT)))
#define EXPANDED_OBJC(ob) \
  (CHECK_TYPE_INLINE(ob, Object *), (((ob)->nlaflag & OB_ADS_COLLAPSED) == 0))
/* 'Sub-object' channels (flags stored in Data block) */
#define FILTER_SKE_OBJD(key) (CHECK_TYPE_INLINE(key, Key *), ((key->flag & KEY_DS_EXPAND)))
#define FILTER_MAT_OBJD(ma) (CHECK_TYPE_INLINE(ma, Material *), ((ma->flag & MA_DS_EXPAND)))
#define FILTER_LAM_OBJD(la) (CHECK_TYPE_INLINE(la, Light *), ((la->flag & LA_DS_EXPAND)))
#define FILTER_CAM_OBJD(ca) (CHECK_TYPE_INLINE(ca, Camera *), ((ca->flag & CAM_DS_EXPAND)))
#define FILTER_CACHEFILE_OBJD(cf) \
  (CHECK_TYPE_INLINE(cf, CacheFile *), (((cf)->flag & CACHEFILE_DS_EXPAND)))
#define FILTER_CUR_OBJD(cu) (CHECK_TYPE_INLINE(cu, Curve *), ((cu->flag & CU_DS_EXPAND)))
#define FILTER_PART_OBJD(part) \
  (CHECK_TYPE_INLINE(part, ParticleSettings *), (((part)->flag & PART_DS_EXPAND)))
#define FILTER_MBALL_OBJD(mb) (CHECK_TYPE_INLINE(mb, MetaBall *), ((mb->flag2 & MB_DS_EXPAND)))
#define FILTER_ARM_OBJD(arm) (CHECK_TYPE_INLINE(arm, bArmature *), ((arm->flag & ARM_DS_EXPAND)))
#define FILTER_MESH_OBJD(me) (CHECK_TYPE_INLINE(me, Mesh *), ((me->flag & ME_DS_EXPAND)))
#define FILTER_LATTICE_OBJD(lt) (CHECK_TYPE_INLINE(lt, Lattice *), ((lt->flag & LT_DS_EXPAND)))
#define FILTER_SPK_OBJD(spk) (CHECK_TYPE_INLINE(spk, Speaker *), ((spk->flag & SPK_DS_EXPAND)))
#define FILTER_CURVES_OBJD(ha) (CHECK_TYPE_INLINE(ha, Curves *), ((ha->flag & HA_DS_EXPAND)))
#define FILTER_POINTS_OBJD(pt) (CHECK_TYPE_INLINE(pt, PointCloud *), ((pt->flag & PT_DS_EXPAND)))
#define FILTER_VOLUME_OBJD(vo) (CHECK_TYPE_INLINE(vo, Volume *), ((vo->flag & VO_DS_EXPAND)))
#define FILTER_SIMULATION_OBJD(sim) \
  (CHECK_TYPE_INLINE(sim, Simulation *), ((sim->flag & SIM_DS_EXPAND)))
/* Variable use expanders */
#define FILTER_NTREE_DATA(ntree) \
  (CHECK_TYPE_INLINE(ntree, bNodeTree *), (((ntree)->flag & NTREE_DS_EXPAND)))
#define FILTER_TEX_DATA(tex) (CHECK_TYPE_INLINE(tex, Tex *), ((tex->flag & TEX_DS_EXPAND)))

/* 'Sub-object/Action' channels (flags stored in Action) */
#define SEL_ACTC(actc) ((actc->flag & ACT_SELECTED))
#define EXPANDED_ACTC(actc) ((actc->flag & ACT_COLLAPSED) == 0)
/* 'Sub-AnimData' channels */
#define EXPANDED_DRVD(adt) ((adt->flag & ADT_DRIVERS_COLLAPSED) == 0)

/* Actions (also used for Dopesheet) */
/* Action Channel Group */
#define EDITABLE_AGRP(agrp) (((agrp)->flag & AGRP_PROTECTED) == 0)
#define EXPANDED_AGRP(ac, agrp) \
  (((!(ac) || ((ac)->spacetype != SPACE_GRAPH)) && ((agrp)->flag & AGRP_EXPANDED)) || \
   (((ac) && ((ac)->spacetype == SPACE_GRAPH)) && ((agrp)->flag & AGRP_EXPANDED_G)))
#define SEL_AGRP(agrp) (((agrp)->flag & AGRP_SELECTED) || ((agrp)->flag & AGRP_ACTIVE))
/* F-Curve Channels */
#define EDITABLE_FCU(fcu) ((fcu->flag & FCURVE_PROTECTED) == 0)
#define SEL_FCU(fcu) (fcu->flag & FCURVE_SELECTED)

/* ShapeKey mode only */
#define EDITABLE_SHAPEKEY(kb) ((kb->flag & KEYBLOCK_LOCKED) == 0)
#define SEL_SHAPEKEY(kb) (kb->flag & KEYBLOCK_SEL)

/* Grease Pencil only */
/* Grease Pencil datablock settings */
#define EXPANDED_GPD(gpd) (gpd->flag & GP_DATA_EXPAND)
/* Grease Pencil Layer settings */
#define EDITABLE_GPL(gpl) ((gpl->flag & GP_LAYER_LOCKED) == 0)
#define SEL_GPL(gpl) (gpl->flag & GP_LAYER_SELECT)

/* Mask Only */
/* Grease Pencil datablock settings */
#define EXPANDED_MASK(mask) (mask->flag & MASK_ANIMF_EXPAND)
/* Grease Pencil Layer settings */
#define EDITABLE_MASK(masklay) ((masklay->flag & MASK_LAYERFLAG_LOCKED) == 0)
#define SEL_MASKLAY(masklay) (masklay->flag & SELECT)

/* NLA only */
#define SEL_NLT(nlt) (nlt->flag & NLATRACK_SELECTED)
#define EDITABLE_NLT(nlt) ((nlt->flag & NLATRACK_PROTECTED) == 0)

/* Movie clip only */
#define EXPANDED_MCLIP(clip) (clip->flag & MCLIP_DATA_EXPAND)

/* Palette only */
#define EXPANDED_PALETTE(palette) (palette->flag & PALETTE_DATA_EXPAND)

/* AnimData - NLA mostly... */
#define SEL_ANIMDATA(adt) (adt->flag & ADT_UI_SELECTED)

/** \} */

/* -------------------------------------------------------------------- */
/** \name Channel Defines
 * \{ */

/* channel heights */
#define ACHANNEL_FIRST_TOP(ac) \
  (UI_view2d_scale_get_y(&(ac)->region->v2d) * -UI_TIME_SCRUB_MARGIN_Y - ACHANNEL_SKIP)
#define ACHANNEL_HEIGHT(ac) (0.8f * (ac)->yscale_fac * U.widget_unit)
#define ACHANNEL_SKIP (0.1f * U.widget_unit)
#define ACHANNEL_STEP(ac) (ACHANNEL_HEIGHT(ac) + ACHANNEL_SKIP)
/* Additional offset to give some room at the end. */
#define ACHANNEL_TOT_HEIGHT(ac, item_amount) \
  (-ACHANNEL_FIRST_TOP(ac) + ACHANNEL_STEP(ac) * (item_amount + 1))

/* channel widths */
#define ACHANNEL_NAMEWIDTH (10 * U.widget_unit)

/* channel toggle-buttons */
#define ACHANNEL_BUTTON_WIDTH (0.8f * U.widget_unit)

/** \} */

/* -------------------------------------------------------------------- */
/** \name NLA Channel Defines
 * \{ */

/* NLA channel heights */
#define NLACHANNEL_FIRST_TOP(ac) \
  (UI_view2d_scale_get_y(&(ac)->region->v2d) * -UI_TIME_SCRUB_MARGIN_Y - NLACHANNEL_SKIP)
#define NLACHANNEL_HEIGHT(snla) \
  (((snla) && ((snla)->flag & SNLA_NOSTRIPCURVES)) ? (0.8f * U.widget_unit) : \
                                                     (1.2f * U.widget_unit))
#define NLACHANNEL_SKIP (0.1f * U.widget_unit)
#define NLACHANNEL_STEP(snla) (NLACHANNEL_HEIGHT(snla) + NLACHANNEL_SKIP)
/* Additional offset to give some room at the end. */
#define NLACHANNEL_TOT_HEIGHT(ac, item_amount) \
  (-NLACHANNEL_FIRST_TOP(ac) + NLACHANNEL_STEP(((SpaceNla *)(ac)->sl)) * (item_amount + 1))

/* channel widths */
#define NLACHANNEL_NAMEWIDTH (10 * U.widget_unit)

/* channel toggle-buttons */
#define NLACHANNEL_BUTTON_WIDTH (0.8f * U.widget_unit)

/** \} */

/* -------------------------------------------------------------------- */
/** \name Public API
 * \{ */

/**
 * This function filters the active data source to leave only animation channels suitable for
 * usage by the caller. It will return the length of the list
 *
 * \param anim_data: Is a pointer to a #ListBase,
 * to which the filtered animation channels will be placed for use.
 * \param filter_mode: how should the data be filtered - bit-mapping accessed flags.
 */
size_t ANIM_animdata_filter(bAnimContext *ac,
                            ListBase *anim_data,
                            eAnimFilter_Flags filter_mode,
                            void *data,
                            eAnimCont_Types datatype);

/**
 * Obtain current anim-data context from Blender Context info
 * - AnimContext to write to is provided as pointer to var on stack so that we don't have
 *   allocation/freeing costs (which are not that avoidable with channels).
 * - Clears data and sets the information from Blender Context which is useful
 * \return whether the operation was successful.
 */
bool ANIM_animdata_get_context(const struct bContext *C, bAnimContext *ac);

/**
 * Obtain current anim-data context,
 * given that context info from Blender context has already been set:
 * - AnimContext to write to is provided as pointer to var on stack so that we don't have
 *   allocation/freeing costs (which are not that avoidable with channels).
 * \return whether the operation was successful.
 */
bool ANIM_animdata_context_getdata(bAnimContext *ac);

/**
 * Acts on bAnimListElem eAnim_Update_Flags.
 */
void ANIM_animdata_update(bAnimContext *ac, ListBase *anim_data);

void ANIM_animdata_freelist(ListBase *anim_data);

/* ************************************************ */
/* ANIMATION CHANNELS LIST */
/* anim_channels_*.c */

/** \} */

/* -------------------------------------------------------------------- */
/** \name Drawing TypeInfo
 * \{ */

/* role or level of animchannel in the hierarchy */
typedef enum eAnimChannel_Role {
  /** datablock expander - a "composite" channel type */
  ACHANNEL_ROLE_EXPANDER = -1,
  /** special purposes - not generally for hierarchy processing */
  /* ACHANNEL_ROLE_SPECIAL = 0, */ /* UNUSED */
  /** data channel - a channel representing one of the actual building blocks of channels */
  ACHANNEL_ROLE_CHANNEL = 1,
} eAnimChannel_Role;

/* flag-setting behavior */
typedef enum eAnimChannels_SetFlag {
  /** turn off */
  ACHANNEL_SETFLAG_CLEAR = 0,
  /** turn on */
  ACHANNEL_SETFLAG_ADD = 1,
  /** on->off, off->on */
  ACHANNEL_SETFLAG_INVERT = 2,
  /** some on -> all off / all on */
  ACHANNEL_SETFLAG_TOGGLE = 3,
} eAnimChannels_SetFlag;

/* types of settings for AnimChannels */
typedef enum eAnimChannel_Settings {
  ACHANNEL_SETTING_SELECT = 0,
  /** warning: for drawing UI's, need to check if this is off (maybe inverse this later) */
  ACHANNEL_SETTING_PROTECT = 1,
  ACHANNEL_SETTING_MUTE = 2,
  ACHANNEL_SETTING_EXPAND = 3,
  /** only for Graph Editor */
  ACHANNEL_SETTING_VISIBLE = 4,
  /** only for NLA Tracks */
  ACHANNEL_SETTING_SOLO = 5,
  /** only for NLA Actions */
  ACHANNEL_SETTING_PINNED = 6,
  ACHANNEL_SETTING_MOD_OFF = 7,
  /** channel is pinned and always visible */
  ACHANNEL_SETTING_ALWAYS_VISIBLE = 8,
} eAnimChannel_Settings;

/* Drawing, mouse handling, and flag setting behavior... */
typedef struct bAnimChannelType {
  /* -- Type data -- */
  /* name of the channel type, for debugging */
  const char *channel_type_name;
  /* "level" or role in hierarchy - for finding the active channel */
  eAnimChannel_Role channel_role;

  /* -- Drawing -- */
  /* get RGB color that is used to draw the majority of the backdrop */
  void (*get_backdrop_color)(bAnimContext *ac, bAnimListElem *ale, float r_color[3]);
  /* draw backdrop strip for channel */
  void (*draw_backdrop)(bAnimContext *ac, bAnimListElem *ale, float yminc, float ymaxc);
  /* get depth of indention (relative to the depth channel is nested at) */
  short (*get_indent_level)(bAnimContext *ac, bAnimListElem *ale);
  /* get offset in pixels for the start of the channel (in addition to the indent depth) */
  short (*get_offset)(bAnimContext *ac, bAnimListElem *ale);

  /* get name (for channel lists) */
  void (*name)(bAnimListElem *ale, char *name);
  /* get RNA property+pointer for editing the name */
  bool (*name_prop)(bAnimListElem *ale, struct PointerRNA *ptr, struct PropertyRNA **prop);
  /* get icon (for channel lists) */
  int (*icon)(bAnimListElem *ale);

  /* -- Settings -- */
  /* check if the given setting is valid in the current context */
  bool (*has_setting)(bAnimContext *ac, bAnimListElem *ale, eAnimChannel_Settings setting);
  /* get the flag used for this setting */
  int (*setting_flag)(bAnimContext *ac, eAnimChannel_Settings setting, bool *neg);
  /* get the pointer to int/short where data is stored,
   * with type being  sizeof(ptr_data) which should be fine for runtime use...
   * - assume that setting has been checked to be valid for current context
   */
  void *(*setting_ptr)(bAnimListElem *ale, eAnimChannel_Settings setting, short *type);
} bAnimChannelType;

/** \} */

/* -------------------------------------------------------------------- */
/** \name Drawing API
 * \{ */

/**
 * Get type info from given channel type.
 */
const bAnimChannelType *ANIM_channel_get_typeinfo(bAnimListElem *ale);

/**
 * Print debug info string for the given channel.
 */
void ANIM_channel_debug_print_info(bAnimListElem *ale, short indent_level);

/**
 * Retrieves the Action associated with this animation channel.
 */
bAction *ANIM_channel_action_get(const bAnimListElem *ale);

/**
 * Draw the given channel.
 */
void ANIM_channel_draw(
    bAnimContext *ac, bAnimListElem *ale, float yminc, float ymaxc, size_t channel_index);
/**
 * Draw UI widgets the given channel.
 */
void ANIM_channel_draw_widgets(const struct bContext *C,
                               bAnimContext *ac,
                               bAnimListElem *ale,
                               struct uiBlock *block,
                               rctf *rect,
                               size_t channel_index);

/** \} */

/* -------------------------------------------------------------------- */
/** \name Editing API
 * \{ */

/**
 * Check if some setting for a channel is enabled
 * Returns: 1 = On, 0 = Off, -1 = Invalid.
 */
short ANIM_channel_setting_get(bAnimContext *ac,
                               bAnimListElem *ale,
                               eAnimChannel_Settings setting);

/**
 * Change value of some setting for a channel.
 */
void ANIM_channel_setting_set(bAnimContext *ac,
                              bAnimListElem *ale,
                              eAnimChannel_Settings setting,
                              eAnimChannels_SetFlag mode);

/**
 * Flush visibility (for Graph Editor) changes up/down hierarchy for changes in the given setting
 * - anim_data: list of the all the anim channels that can be chosen
 *   -> filtered using ANIMFILTER_CHANNELS only, since if we took VISIBLE too,
 *      then the channels under closed expanders get ignored...
 * - ale_setting: the anim channel (not in the anim_data list directly, though occurring there)
 *   with the new state of the setting that we want flushed up/down the hierarchy
 * - setting: type of setting to set
 * - on: whether the visibility setting has been enabled or disabled
 */
void ANIM_flush_setting_anim_channels(bAnimContext *ac,
                                      ListBase *anim_data,
                                      bAnimListElem *ale_setting,
                                      eAnimChannel_Settings setting,
                                      eAnimChannels_SetFlag mode);

/**
 * Set selection state of all animation channels in the context.
 */
void ANIM_anim_channels_select_set(bAnimContext *ac, eAnimChannels_SetFlag sel);

/**
 * Toggle selection state of all animation channels in the context.
 */
void ANIM_anim_channels_select_toggle(bAnimContext *ac);

/**
 * Set the given animation-channel as the active one for the active context.
 */
void ANIM_set_active_channel(bAnimContext *ac,
                             void *data,
                             eAnimCont_Types datatype,
                             eAnimFilter_Flags filter,
                             void *channel_data,
                             eAnim_ChannelType channel_type);

/**
 * Delete the F-Curve from the given AnimData block (if possible),
 * as appropriate according to animation context.
 */
void ANIM_fcurve_delete_from_animdata(bAnimContext *ac, struct AnimData *adt, struct FCurve *fcu);

/**
 * Unlink the action from animdata if it's empty.
 *
 * If the action has no F-Curves, unlink it from AnimData if it did not
 * come from a NLA Strip being tweaked.
 */
bool ANIM_remove_empty_action_from_animdata(struct AnimData *adt);

/* ************************************************ */
/* DRAWING API */
/* anim_draw.c */

/** \} */

/* -------------------------------------------------------------------- */
/** \name Current Frame Drawing
 *
 * Main call to draw current-frame indicator in an Animation Editor.
 * \{ */

/* flags for Current Frame Drawing */
typedef enum eAnimEditDraw_CurrentFrame {
  /* plain time indicator with no special indicators */
  /* DRAWCFRA_PLAIN = 0, */ /* UNUSED */
  /* time indication in seconds or frames */
  DRAWCFRA_UNIT_SECONDS = (1 << 0),
  /* draw indicator extra wide (for timeline) */
  DRAWCFRA_WIDE = (1 << 1),
} eAnimEditDraw_CurrentFrame;

/**
 * General call for drawing current frame indicator in animation editor.
 */
void ANIM_draw_cfra(const struct bContext *C, struct View2D *v2d, short flag);

/** \} */

/* -------------------------------------------------------------------- */
/** \name Preview Range Drawing
 *
 * Main call to draw preview range curtains.
 * \{ */

/**
 * Draw preview range 'curtains' for highlighting where the animation data is.
 */
void ANIM_draw_previewrange(const struct bContext *C, struct View2D *v2d, int end_frame_width);

/** \} */

/* -------------------------------------------------------------------- */
/** \name Frame Range Drawing
 *
 * Main call to draw normal frame range indicators.
 * \{ */

/**
 * Draw frame range guides (for scene frame range) in background.
 *
 * TODO: Should we still show these when preview range is enabled?
 */
void ANIM_draw_framerange(struct Scene *scene, struct View2D *v2d);

/**
 * Draw manually set intended playback frame range guides for the action in the background.
 * Allows specifying a subset of the Y range of the view.
 */
void ANIM_draw_action_framerange(
    struct AnimData *adt, struct bAction *action, struct View2D *v2d, float ymin, float ymax);

/* ************************************************* */
/* F-MODIFIER TOOLS */

/** \} */

/* -------------------------------------------------------------------- */
/** \name UI Panel Drawing
 * \{ */

bool ANIM_nla_context_track_ptr(const struct bContext *C, struct PointerRNA *r_ptr);
bool ANIM_nla_context_strip_ptr(const struct bContext *C, struct PointerRNA *r_ptr);

struct NlaTrack *ANIM_nla_context_track(const struct bContext *C);
struct NlaStrip *ANIM_nla_context_strip(const struct bContext *C);
struct FCurve *ANIM_graph_context_fcurve(const struct bContext *C);

/* Needed for abstraction between the graph editor and the NLA editor. */
typedef bool (*PanelTypePollFn)(const struct bContext *C, struct PanelType *pt);
/* Avoid including "UI_interface.h" here. */
typedef void (*uiListPanelIDFromDataFunc)(void *data_link, char *r_idname);

/**
 * Checks if the panels match the active strip / curve, rebuilds them if they don't.
 */
void ANIM_fmodifier_panels(const struct bContext *C,
                           struct ID *owner_id,
                           struct ListBase *fmodifiers,
                           uiListPanelIDFromDataFunc panel_id_fn);

void ANIM_modifier_panels_register_graph_and_NLA(struct ARegionType *region_type,
                                                 const char *modifier_panel_prefix,
                                                 PanelTypePollFn poll_function);
void ANIM_modifier_panels_register_graph_only(struct ARegionType *region_type,
                                              const char *modifier_panel_prefix,
                                              PanelTypePollFn poll_function);

/** \} */

/* -------------------------------------------------------------------- */
/** \name Copy/Paste Buffer
 * \{ */

/**
 * Free the copy/paste buffer.
 */
void ANIM_fmodifiers_copybuf_free(void);

/**
 * Copy the given F-Modifiers to the buffer, returning whether anything was copied or not
 * assuming that the buffer has been cleared already with #ANIM_fmodifiers_copybuf_free()
 * \param active: Only copy the active modifier.
 */
bool ANIM_fmodifiers_copy_to_buf(ListBase *modifiers, bool active);

/**
 * 'Paste' the F-Modifier(s) from the buffer to the specified list
 * \param replace: Free all the existing modifiers to leave only the pasted ones.
 */
bool ANIM_fmodifiers_paste_from_buf(ListBase *modifiers, bool replace, struct FCurve *curve);

/* ************************************************* */
/* ASSORTED TOOLS */

/** \} */

/* -------------------------------------------------------------------- */
/** \name Animation F-Curves <-> Icons/Names Mapping
 * \{ */

/* anim_ipo_utils.c */

/**
 * Get icon + name for channel-list displays for F-Curve.
 *
 * Write into "name" buffer, the name of the property
 * (retrieved using RNA from the curve's settings),
 * and return the icon used for the struct that this property refers to
 *
 * \warning name buffer we're writing to cannot exceed 256 chars
 * (check anim_channels_defines.c for details).
 */
int getname_anim_fcurve(char *name, struct ID *id, struct FCurve *fcu);

/**
 * Automatically determine a color for the nth F-Curve.
 */
void getcolor_fcurve_rainbow(int cur, int tot, float out[3]);

/** \} */

/* -------------------------------------------------------------------- */
/** \name NLA Drawing
 *
 * \note Technically, this is not in the animation module (it's in space_nla)
 * but these are sometimes needed by various animation API's.
 * \{ */

/**
 * Get color to use for NLA Action channel's background.
 * \note color returned includes fine-tuned alpha!
 */
void nla_action_get_color(struct AnimData *adt, struct bAction *act, float color[4]);

/** \} */

/* -------------------------------------------------------------------- */
/** \name NLA-Mapping
 * \{ */

/* anim_draw.c */

/**
 * Obtain the AnimData block providing NLA-mapping for the given channel (if applicable).
 *
 * TODO: do not supply return this if the animdata tells us that there is no mapping to perform.
 */
struct AnimData *ANIM_nla_mapping_get(bAnimContext *ac, bAnimListElem *ale);

/**
 * Apply/Unapply NLA mapping to all keyframes in the nominated F-Curve
 * \param restore: Whether to map points back to non-mapped time.
 * \param only_keys: Whether to only adjust the location of the center point of beztriples.
 */
void ANIM_nla_mapping_apply_fcurve(struct AnimData *adt,
                                   struct FCurve *fcu,
                                   bool restore,
                                   bool only_keys);

/* ..... */

/**
 * Perform validation & auto-blending/extend refreshes after some operations
 * \note defined in space_nla/nla_edit.c, not in animation/
 */
void ED_nla_postop_refresh(bAnimContext *ac);

/** \} */

/* -------------------------------------------------------------------- */
/** \name Unit Conversion Mappings
 * \{ */

/* anim_draw.c */

/* flags for conversion mapping */
typedef enum eAnimUnitConv_Flags {
  /* restore to original internal values */
  ANIM_UNITCONV_RESTORE = (1 << 0),
  /* ignore handles (i.e. only touch main keyframes) */
  ANIM_UNITCONV_ONLYKEYS = (1 << 1),
  /* only touch selected BezTriples */
  ANIM_UNITCONV_ONLYSEL = (1 << 2),
  /* only touch selected vertices */
  ANIM_UNITCONV_SELVERTS = (1 << 3),
  /* ANIM_UNITCONV_SKIPKNOTS = (1 << 4), */ /* UNUSED */
  /* Scale FCurve i a way it fits to -1..1 space */
  ANIM_UNITCONV_NORMALIZE = (1 << 5),
  /* Only when normalization is used: use scale factor from previous run,
   * prevents curves from jumping all over the place when tweaking them.
   */
  ANIM_UNITCONV_NORMALIZE_FREEZE = (1 << 6),
} eAnimUnitConv_Flags;

/**
 * Get flags used for normalization in ANIM_unit_mapping_get_factor.
 */
short ANIM_get_normalization_flags(bAnimContext *ac);
/**
 * Get unit conversion factor for given ID + F-Curve.
 */
float ANIM_unit_mapping_get_factor(
    struct Scene *scene, struct ID *id, struct FCurve *fcu, short flag, float *r_offset);

/** \} */

/* -------------------------------------------------------------------- */
/** \name Utility macros
 * \{ */

/**
 * Provide access to Keyframe Type info in #BezTriple.
 * NOTE: this is so that we can change it from being stored in 'hide'
 */
#define BEZKEYTYPE(bezt) ((bezt)->hide)

/* set/clear/toggle macro
 * - channel - channel with a 'flag' member that we're setting
 * - smode - 0=clear, 1=set, 2=invert
 * - sflag - bitflag to set
 */
#define ACHANNEL_SET_FLAG(channel, smode, sflag) \
  { \
    if (smode == ACHANNEL_SETFLAG_INVERT) { \
      (channel)->flag ^= (sflag); \
    } \
    else if (smode == ACHANNEL_SETFLAG_ADD) { \
      (channel)->flag |= (sflag); \
    } \
    else { \
      (channel)->flag &= ~(sflag); \
    } \
  } \
  ((void)0)

/* set/clear/toggle macro, where the flag is negative
 * - channel - channel with a 'flag' member that we're setting
 * - smode - 0=clear, 1=set, 2=invert
 * - sflag - bitflag to set
 */
#define ACHANNEL_SET_FLAG_NEG(channel, smode, sflag) \
  { \
    if (smode == ACHANNEL_SETFLAG_INVERT) { \
      (channel)->flag ^= (sflag); \
    } \
    else if (smode == ACHANNEL_SETFLAG_ADD) { \
      (channel)->flag &= ~(sflag); \
    } \
    else { \
      (channel)->flag |= (sflag); \
    } \
  } \
  ((void)0)

/** \} */

/* anim_deps.c */

/* -------------------------------------------------------------------- */
/** \name Animation Updates
 * \{ */

/**
 * Tags the given ID block for refreshes (if applicable) due to Animation Editor editing.
 */
void ANIM_id_update(struct Main *bmain, struct ID *id);
/**
 * Tags the given anim list element for refreshes (if applicable) due to Animation Editor editing.
 */
void ANIM_list_elem_update(struct Main *bmain, struct Scene *scene, bAnimListElem *ale);

/* data -> channels syncing */

/**
 * Main call to be exported to animation editors.
 */
void ANIM_sync_animchannels_to_data(const struct bContext *C);

void ANIM_center_frame(struct bContext *C, int smooth_viewtx);

/** \} */

/* -------------------------------------------------------------------- */
/** \name Operators
 * \{ */

/* generic animation channels */
void ED_operatortypes_animchannels(void);
void ED_keymap_animchannels(struct wmKeyConfig *keyconf);

/* generic time editing */
void ED_operatortypes_anim(void);
void ED_keymap_anim(struct wmKeyConfig *keyconf);

/* space_graph */
void ED_operatormacros_graph(void);
/* space_action */
void ED_operatormacros_action(void);

/** \} */

/* -------------------------------------------------------------------- */
/** \name Animation Editor Exports
 * \{ */

/* XXX: Should we be doing these here, or at all? */

/**
 * Action Editor - Action Management.
 * Helper function to find the active AnimData block from the Action Editor context.
 */
struct AnimData *ED_actedit_animdata_from_context(const struct bContext *C,
                                                  struct ID **r_adt_id_owner);
void ED_animedit_unlink_action(struct bContext *C,
                               struct ID *id,
                               struct AnimData *adt,
                               struct bAction *act,
                               struct ReportList *reports,
                               bool force_delete);

/**
 * Set up UI configuration for Drivers Editor
 * (drivers editor window) and RNA (mode switching).
 * \note Currently called from window-manager.
 */
void ED_drivers_editor_init(struct bContext *C, struct ScrArea *area);

/* ************************************************ */

typedef enum eAnimvizCalcRange {
  /* Update motion paths at the current frame only. */
  ANIMVIZ_CALC_RANGE_CURRENT_FRAME,

  /* Try to limit updates to a close neighborhood of the current frame. */
  ANIMVIZ_CALC_RANGE_CHANGED,

  /* Update an entire range of the motion paths. */
  ANIMVIZ_CALC_RANGE_FULL,
} eAnimvizCalcRange;

struct Depsgraph *animviz_depsgraph_build(struct Main *bmain,
                                          struct Scene *scene,
                                          struct ViewLayer *view_layer,
                                          struct ListBase *targets);

void animviz_calc_motionpaths(struct Depsgraph *depsgraph,
                              struct Main *bmain,
                              struct Scene *scene,
                              ListBase *targets,
                              eAnimvizCalcRange range,
                              bool restore);

/**
 * Get list of motion paths to be baked for the given object.
 * - assumes the given list is ready to be used.
 */
void animviz_get_object_motionpaths(struct Object *ob, ListBase *targets);

/** \} */

#ifdef __cplusplus
}
#endif
