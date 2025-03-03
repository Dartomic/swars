#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "bflib_video.h"
#include "bfdata.h"
#include "bfsprite.h"
#include "bflib_video.h"
#include "bflib_keybrd.h"
#include "bflib_mouse.h"
#include "bflib_render.h"
#include "bfpalette.h"
#include "bfmemory.h"
#include "bfmemut.h"
#include "bffile.h"
#include "game_data.h"
#include "display.h"
#include "dos.h"
#include "game.h"
#include "keyboard.h"
#include "mouse.h"
#include "sound.h"
#include "unix.h"
#include "util.h"
#include "windows.h"

#include "timer.h"

#define SAVEGAME_PATH "qdata/savegame/"

#pragma pack(1)

struct UnkStruct7
{
  char field_0;
  char field_1;
  char field_2;
  char field_3;
  char field_4;
  char field_5;
  char field_6;
  char field_7;
  char field_8;
  char field_9;
  char field_A;
  char field_B;
  char field_C;
  char field_D;
  char field_E;
  char field_F;
};

#pragma pack()

extern uint8_t game_music_track;

extern struct TbLoadFiles unk02_load_files[15];

extern TbFileHandle packet_rec_fh;

unsigned int LbRandomAnyShort(void);

int LbPaletteFade(uint8_t *a1, uint8_t a2, uint8_t a3)
{
    int ret;
    asm volatile ("call ASM_LbPaletteFade\n"
        : "=r" (ret) : "a" (a1), "d" (a2), "b" (a3));
    return ret;
}

TbResult LbScreenUnlock(void)
{
    return Lb_SUCCESS;
}

TbResult LbScreenLock(void)
{
    return Lb_SUCCESS;
}

TbResult ASM_LbScreenSwap(void);

TbResult LbScreenSwap(void)
{
    return ASM_LbScreenSwap();
}

TbResult LbScreenSwapClear(TbPixel colour)
{
    int ret;
    asm volatile ("call ASM_LbScreenSwapClear\n"
        : "=r" (ret) : "a" (colour));
    return ret;
}

void PacketRecord_Close(void)
{
    if (in_network_game)
        return;
    LbFileClose(packet_rec_fh);
}

bool
game_initialise(void)
{
    if (SDL_Init (SDL_INIT_JOYSTICK | SDL_INIT_VIDEO
          | SDL_INIT_NOPARACHUTE) != 0)
    {
        ERRORLOG("SDL_Init(): %s", SDL_GetError());
        return false;
    }

#ifdef __unix__
    /* clean up after SDL messing around where it shouldn't */
    unix_restore_signal_handlers();
#endif

    keyboard_initialise();
    mouse_initialise();
    display_initialise();
    sound_initialise();

    // Make sure file names are properly converted before opening
    setup_file_names();

    if ( cmdln_param_w == 1 )
    {
        buffer_allocs[35].field_A = 1;
        buffer_allocs[28].field_A = 1;
        buffer_allocs[36].field_A = 1;
        buffer_allocs[27].field_A = 1000;
        buffer_allocs[26].field_A = 1124;
        buffer_allocs[31].field_A = 2500;
        buffer_allocs[32].field_A = 1000;
        buffer_allocs[33].field_A = 700;
        buffer_allocs[30].field_A = 3000;
        if ( cmdln_param_mp || cmdln_param_bcg )
        {
            buffer_allocs[20].field_A = 2000;
            buffer_allocs[21].field_A = 2000;
            buffer_allocs[22].field_A = 2000;
        }
        buffer_allocs[4].field_A = 11000;
        buffer_allocs[9].field_A = 11000;
        buffer_allocs[5].field_A = 1500;
        buffer_allocs[7].field_A = 1000;
        buffer_allocs[13].field_A = 16000;
        buffer_allocs[14].field_A = 9000;
        engine_mem_alloc_size = 2700000;
        game_perspective = (buffer_allocs[5].field_A >> 8) & 0xff;
    }
    if ( !cmdln_param_mp )
        cmdln_param_bcg = 1;

    return true;
}

void
game_handle_sdl_events (void)
{
    const size_t max_events = 256;
    size_t event;
    SDL_Event ev;

    for (event = 0; event < max_events; event++)
    {
        if (SDL_PollEvent(&ev) == 0)
            break;

        switch (ev.type)
        {
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
            mouse_handle_event(&ev);
            break;

        case SDL_KEYUP:
        case SDL_KEYDOWN:
            keyboard_handle_event(&ev);
            break;

        case SDL_QUIT:
            game_quit();
        }
    }
}

void ASM_game_setup(void);
void ASM_load_texturemaps(void);
void ASM_load_prim_quad(void);
void ASM_game_setup_sub1(void);
void ASM_init_arrays_1(void);
void ASM_game_setup_sub3(void);
void ASM_game_setup_sub5(void);
void ASM_setup_host(void);
void ASM_read_user_settings(void);
void ASM_setup_color_lookups(void);
void ASM_game_setup_sub7(void);
void ASM_game_setup_sub8(void);
void ASM_init_engine(void);
void ASM_swap_wscreen(void);
void ASM_play_intro(void);
void ASM_init_syndwars(void);
void ASM_setup_host_sub6(void);
int ASM_setup_mele(void);
int ASM_LbIKeyboardOpen(void);

extern unsigned char *fade_data;
extern char *fadedat_fname;
extern char *pop_dat_fname_fmt;
extern char *pop_tab_fname_fmt;
extern unsigned short ingame__draw_unknprop_01;
extern unsigned long unkn_buffer_04;

extern struct TbSprite *small_font;
extern struct TbSprite *small_font_end;
extern unsigned char *font0_data;
extern struct TbSprite *pointer_sprites;
extern struct TbSprite *pointer_sprites_end;
extern unsigned char *pointer_data;
extern struct TbSprite *pop1_sprites;
extern struct TbSprite *pop1_sprites_end;
extern unsigned char *pop1_data;
extern struct TbSprite *m_sprites;
extern struct TbSprite *m_sprites_end;
extern unsigned char *m_spr_data;

extern unsigned short ingame__DisplayMode;
extern unsigned char *display_palette;
extern unsigned short unkn2_pos_x;
extern unsigned short unkn2_pos_y;
extern unsigned short unkn2_pos_z;
extern int data_1c8428;
extern const char *primvehobj_fname;
extern unsigned char data_19ec6f;
extern unsigned char textwalk_data[640];

extern PrimObjectPoint *prim_object_points;
extern PrimObjectFace *prim_object_faces;
extern PrimObjectFace4 *prim_object_faces4;
extern PrimObject *prim_objects;
extern Prim4Texture *prim4_textures;
extern PrimFaceTexture *prim_face_textures;

extern ushort prim_object_points_count;
extern ushort prim_object_faces_count;
extern ushort prim_object_faces4_count;
extern ushort prim_objects_count;
extern ushort prim4_textures_count;
extern ushort prim_face_textures_count;
extern ushort prim_unknprop01;
extern struct UnkStruct7 *game_panel;
extern struct UnkStruct7 game_panel_lo[];
extern struct UnkStruct7 unknstrct7_arr2[];

extern uint8_t execute_commands;
extern long gamep_unknval_10;
extern long gamep_unknval_11;
extern long gamep_unknval_12;
extern long gamep_unknval_13;
extern long gamep_unknval_14;
extern long gamep_unknval_15;
extern long gamep_unknval_16;


extern int8_t ingame__TrenchcoatPreference;
extern int8_t ingame__PanelPermutation;

void *ASM_smack_malloc(int msize);
void ASM_smack_mfree(void *ptr);
void *(*smack_malloc)(int);
void (*smack_free)(void *);


void load_texturemaps(void)
{
    ASM_load_texturemaps();
}

void test_open(int num)
{
    // Empty for production version
}

void read_textwalk(void)
{
    TbFileHandle handle;
    handle = LbFileOpen("data/textwalk.dat", Lb_FILE_MODE_READ_ONLY);
    if ( handle != INVALID_FILE )
    {
        LbFileRead(handle, textwalk_data, 640);
        LbFileClose(handle);
    }
}

void read_primveh_obj(const char *fname, int a2)
{
    long firstval;
    TbFileHandle fh;

    fh = LbFileOpen(fname, Lb_FILE_MODE_READ_ONLY);
    if ( fh == INVALID_FILE )
        return;
    LbFileRead(fh, &firstval, sizeof(long));
    if ( firstval != 1 )
    {
      LbFileRead(fh, &prim_object_points_count, sizeof(ushort));
      LbFileRead(fh, &prim_object_faces_count, sizeof(ushort));
      LbFileRead(fh, &prim_object_faces4_count, sizeof(ushort));
      LbFileRead(fh, &prim_objects_count, sizeof(ushort));
      LbFileRead(fh, &prim4_textures_count, sizeof(ushort));
      LbFileRead(fh, &prim_face_textures_count, sizeof(ushort));
      LbFileRead(fh, &prim_unknprop01, sizeof(ushort));
      LbFileRead(fh, prim_object_points, sizeof(PrimObjectPoint) * prim_object_points_count);
      LbFileRead(fh, prim_object_faces, sizeof(PrimObjectFace) * prim_object_faces_count);
      LbFileRead(fh, prim_object_faces4, sizeof(PrimObjectFace4) * prim_object_faces4_count);
      LbFileRead(fh, prim_objects, sizeof(PrimObject) * prim_objects_count);
      LbFileRead(fh, prim4_textures, sizeof(Prim4Texture) * prim4_textures_count);
      LbFileRead(fh, prim_face_textures, sizeof(PrimFaceTexture) * prim_face_textures_count);
    }
    LbFileClose(fh);
}

void load_prim_quad(void)
{
    unkn2_pos_x = 64;
    unkn2_pos_y = 64;
    unkn2_pos_z = 64;
    data_1c8428 = 0;
    prim_unknprop01 = 1000;
    read_primveh_obj(primvehobj_fname, 1);
    read_textwalk();
    data_19ec6f = 1;
    ingame__DisplayMode = 55;
    if ( cmdln_param_bcg == 99 )
        test_open(99);
    if ( cmdln_param_bcg == 100 )
        test_open(100);
}

void game_setup_sub1(void)
{
    ASM_game_setup_sub1();
}

void init_arrays_1(void)
{
    ASM_init_arrays_1();
}

void game_setup_sub2(int a1)
{
    asm volatile ("call ASM_game_setup_sub2\n"
        : : "a" (a1));
}

void game_setup_sub3(void)
{
    ASM_game_setup_sub3();
}

void game_setup_sub4(int a1)
{
    test_open(0);
}

void game_setup_sub5(void)
{
    ASM_game_setup_sub5();
}

void play_intro(void)
{
    ASM_play_intro();
}

int LbGhostTableGenerate(TbPixel *pal, int a2, char *fname)
{
    int ret;
    asm volatile ("call ASM_LbGhostTableGenerate\n"
        : "=r" (ret) : "a" (pal), "d" (a2), "b" (fname));
    return ret;
}

int LbKeyboardOpen(void)
{
    return 1;
}

int LbIKeyboardOpen(void)
{
    ASM_LbIKeyboardOpen();
    return 1;
}

int setup_host_sub5(BuffUnknStruct02 *a1)
{
    int ret;
    asm volatile ("call ASM_setup_host_sub5\n"
        : "=r" (ret) : "a" (a1));
    return ret;
}

int LoadSounds(unsigned char a1)
{
    int ret;
    asm volatile ("call ASM_LoadSounds\n"
        : "=r" (ret) : "a" (a1));
    return ret;
}

int LoadMusic(unsigned char a1)
{
    int ret;
    asm volatile ("call ASM_LoadMusic\n"
        : "=r" (ret) : "a" (a1));
    return ret;
}

void init_syndwars(void)
{
    ASM_init_syndwars();
}

void setup_host_sub6(void)
{
    ASM_setup_host_sub6();
}

int setup_mele(void)
{
    return ASM_setup_mele();
}

void set_smack_malloc(void *(*cb)(int))
{
    smack_malloc = cb;
}

void set_smack_free(void (*cb)(void *ptr))
{
    smack_free = cb;
}

void BAT_unknsub_20(int a1, int a2, int a3, int a4, unsigned long a5)
{
    asm volatile ("call ASM_BAT_unknsub_20\n"
        : : "a" (a1), "d" (a2), "b" (a3), "c" (a4), "g" (a5));
}

void setup_host(void)
{
    //ASM_setup_host(); return;
    char fname[DISKPATH_SIZE];
    BAT_unknsub_20(0, 0, 0, 0, unkn_buffer_04 + 41024);
    set_smack_malloc(ASM_smack_malloc);
    set_smack_free(ASM_smack_mfree);
    DEBUGLOG(0,"&setup_host() = 0x%lx", (ulong)setup_host);
    lbDisplay.ScreenMode = Lb_SCREEN_MODE_320_200_8;
    LbScreenSetup(lbDisplay.ScreenMode, 320, 200, display_palette);
    LbSpriteSetup(pointer_sprites, pointer_sprites_end, pointer_data);
    {
        struct TbSprite *spr;
        spr = &pointer_sprites[1];
        spr->SWidth = 0;
        spr->SHeight = 0;
    }
    if ( cmdln_param_d )
        LbKeyboardOpen();
    else
        LbIKeyboardOpen();
    LbMouseSetup(&pointer_sprites[1], 2, 2);
    setup_mele();
    LbSpriteSetup(m_sprites, m_sprites_end, m_spr_data);
    ingame__PanelPermutation = -2;
    {
        int file_len;
        sprintf(fname, pop_dat_fname_fmt, 1);
        LbFileLoadAt(fname, pop1_data);
        sprintf(fname, pop_tab_fname_fmt, -ingame__PanelPermutation - 1);
        file_len = LbFileLoadAt(fname, pop1_sprites);
        pop1_sprites_end = &pop1_sprites[file_len/sizeof(struct TbSprite)];
        LbSpriteSetup(pop1_sprites, pop1_sprites_end, pop1_data);
    }
    ingame__TrenchcoatPreference = 0;
    game_panel = game_panel_lo;
    LbGhostTableGenerate(display_palette, 50, "data/synghost.tab");
    setup_host_sub5(buffer_allocs);
    init_syndwars();
    LoadSounds(0);
    LoadMusic(0);
    setup_host_sub6();
    if ( pktrec_mode == 1 )
    {
      if ( !in_network_game )
      {
          int file_no;
          file_no = get_new_packet_record_no(selected_map_index);
          get_packet_record_fname(fname, selected_map_index, file_no+1);
          DEBUGLOG(0,"%s: Opening for packet save", fname);
          packet_rec_fh = LbFileOpen(fname, Lb_FILE_MODE_NEW);
          LbFileWrite(packet_rec_fh, &cmdln_param_current_map, 2);
      }
    }
    if ( pktrec_mode == 2 )
    {
        ushort pktrec_head;
        get_packet_record_fname(fname, selected_map_index, cmdln_pr_num);
        DEBUGLOG(0,"%s: Opening for packet input", fname);
        packet_rec_fh = LbFileOpen(fname, Lb_FILE_MODE_READ_ONLY);
        LbFileRead(packet_rec_fh, &pktrec_head, sizeof(pktrec_head));
    }
    play_intro();
    if ( cmdln_param_bcg )
    {
        lbDisplay.ScreenMode = Lb_SCREEN_MODE_640_480_8;
        LbScreenSetup(lbDisplay.ScreenMode, 640, 480, display_palette);
    }
    LbMouseSetup(&pointer_sprites[1], 2, 2);
    if ( cmdln_param_bcg )
      LbMouseChangeSprite(0);
}

void read_user_settings(void)
{
    ASM_read_user_settings();
}

void setup_color_lookups(void)
{
    ASM_setup_color_lookups();
}

void game_setup_sub7(void)
{
    ASM_game_setup_sub7();
}

void game_setup_sub8(void)
{
    ASM_game_setup_sub8();
}

void load_mission_file(int num)
{
    asm volatile ("call ASM_load_mission_file\n"
        : : "a" (num));
}

void init_engine(void)
{
    ASM_init_engine();
}

void load_mission_map_lvl(unsigned char num)
{
    asm volatile ("call ASM_load_mission_map_lvl\n"
        : : "a" (num));
}

void swap_wscreen(void)
{
    TbBool has_wscreeen;
    has_wscreeen = (lbDisplay.WScreen != 0);
    if ( has_wscreeen )
        LbScreenUnlock();
    LbScreenSwap();
    if ( has_wscreeen )
    {
      while ( LbScreenLock() != 1 )
        ;
    }
}

char *gui_strings_data_end;

TbBool create_strings_list(char **strings, char *strings_data, char *strings_data_end)
{
  int text_idx;
  char *text_ptr;
  char **text_arr;
  text_arr = strings;
  text_idx = STRINGS_MAX;
  text_ptr = strings_data;
  while (text_idx >= 0)
  {
      if (text_ptr >= strings_data_end) {
          break;
      }
      *text_arr = text_ptr;
      text_arr++;
      char chr_prev;
      do {
          chr_prev = *text_ptr;
          text_ptr++;
      } while ((chr_prev != '\0') && (text_ptr < strings_data_end));
      text_idx--;
  }
  return (text_idx < STRINGS_MAX);
}

void create_tables_file(void)
{
    int i, k;
    unsigned char *curbuf;
    LbFileLoadAt(fadedat_fname, fade_data);
    curbuf = fade_data;
    for (i = 0; i < 256; i++) {
        for (k = 0; k < 64; k++) {
          _fade_table[256 * k + i] = *(curbuf + 320 * k + i);
        }
    }
    fade_data = curbuf;
    LbFileSaveAt("data/tables.dat", _fade_table, 14900);
}

void game_setup(void)
{
    //ASM_game_setup();return;

    engine_mem_alloc_ptr = LbMemoryAlloc(engine_mem_alloc_size);
    load_texturemaps();
    LbDataLoadAll(unk02_load_files);
    game_setup_sub1();
    init_arrays_1();
    game_setup_sub2(0);
    game_setup_sub3();
    ingame__draw_unknprop_01 = 0;
    game_setup_sub4(-4);
    game_setup_sub5();
    create_strings_list(gui_strings, gui_strings_data, gui_strings_data_end);
    setup_host();
    read_user_settings();
    game_setup_sub4(-3);
    setup_color_lookups();
    game_setup_sub7();
    game_setup_sub4(-2);
    LbSpriteSetup(small_font, small_font_end, font0_data);
    game_setup_sub8();
    load_mission_file(0);
    players[local_player_no].field_BB = 15;
    game_setup_sub4(-1);
    if ( cmdln_param_mp || cmdln_param_bcg )
    {
        load_prim_quad();
    }
    game_setup_sub4(0);
    init_engine();
    if ( !cmdln_param_bcg )
    {
        LbMemorySet(lbDisplay.WScreen, 0, lbDisplay.PhysicalScreenWidth * lbDisplay.PhysicalScreenHeight);
        swap_wscreen();
        LbPaletteSet(display_palette);
    }
    test_open(15);
    game_setup_sub4(1);
    if ( cmdln_param_mp && cmdln_param_current_map )
      load_mission_map_lvl(0);
    if ( in_network_game || cmdln_param_bcg )
      ingame__DisplayMode = 55;
    game_setup_sub4(2);
    if ( cmdln_param_tf == 2 )
    {
        create_tables_file();
    }
}

void game_process_sub01(void)
{
    unsigned long tick_time = clock();
    tick_time = tick_time / 100;
    curr_tick_time = tick_time;
    if (tick_time != prev_tick_time)
    {
        unsigned long tmp;
        tmp = gameturn - prev_gameturn;
        prev_gameturn = gameturn;
        turns_delta = tmp;
    }
    if ( turns_delta != 0 ) {
        fifties_per_gameturn = 800 / turns_delta;
    } else {
        fifties_per_gameturn = 50;
    }
    if ( in_network_game )
        fifties_per_gameturn = 80;
    if ( fifties_per_gameturn > 400 )
        fifties_per_gameturn = 400;
    prev_tick_time = curr_tick_time;
}

void game_process_sub08(void);

void game_process_sub09(void)
{
    int i;
    switch ( gamep_unknval_01 )
    {
    case 1:
        game_process_sub08();
        break;
    case 2:
        for (i = 0; i < 10; i++) {
            ushort pos;
            uint8_t *ptr;
            pos = LbRandomAnyShort() + (gameturn >> 2);
            ptr = &vec_tmap[pos];
            *ptr = unknoise_tmap[*ptr];
        }
        break;
    }
}

void debug_m_sprite(int idx)
{
    int i;
    char strdata[100];
    char *str;
    struct TbSprite *spr;
    unsigned char *ptr;
    spr = &m_sprites[idx];
    str = strdata;
    sprintf(str, "spr %d width %d height %d ptr 0x%lx data",
      idx, (int)spr->SWidth, (int)spr->SHeight, (ulong)spr->Data);
    ptr = spr->Data;
    for (i = 0; i < 10; i++)
    {
        str = strdata + strlen(strdata);
        sprintf(str, " %02x", (int)*ptr);
        ptr++;
    }
    DEBUGLOG(0,"m_sprites: %s", strdata);
}

void mapwho_unkn01(int a1, int a2)
{
    asm volatile ("call ASM_mapwho_unkn01\n"
        : : "a" (a1), "d" (a2));
}

void new_bang(int a1, int a2, int a3, int a4, int a5, int a6)
{
    asm volatile ("call ASM_new_bang\n"
        : : "a" (a1), "d" (a2), "b" (a3), "c" (a4), "g" (a5), "g" (a6));
}

void new_bang_3(int a1, int a2, int a3, int a4)
{
    new_bang(a1, a2, a3, a4, 0, 0);
}

void process_sound_heap(void);

void input(void)
{
    uint16_t n;
    n = lbShift;
    if ( lbKeyOn[KC_LSHIFT] || lbKeyOn[KC_RSHIFT] )
        n |= KMod_SHIFT;
    else
        n &= ~KMod_SHIFT;
    if ( lbKeyOn[KC_LCONTROL] || lbKeyOn[KC_RCONTROL] )
        n |= KMod_CONTROL;
    else
        n &= ~KMod_CONTROL;
    if ( lbKeyOn[KC_RALT] || lbKeyOn[KC_LALT] )
        n |= KMod_ALT;
    else
        n &= ~KMod_ALT;
    lbShift = n;
}

short PlayCDTrack(int a1)
{
    int ret;
    asm volatile ("call ASM_PlayCDTrack\n"
        : "=r" (ret) : "a" (a1));
    return ret;
}

void gproc3_unknsub3(int a1)
{
    // Empty
}

void ASM_gproc3_unknsub1(void);
void gproc3_unknsub1(void)
{
    ASM_gproc3_unknsub1();
}

void ASM_gproc3_unknsub2(void);
void gproc3_unknsub2(void)
{
    ASM_gproc3_unknsub2();
}

void ASM_BAT_play(void);
void BAT_play(void)
{
    ASM_BAT_play();
}

void ASM_show_menu_screen(void);
void show_menu_screen(void)
{
    ASM_show_menu_screen();
}

void game_process_display(void)
{
    //ASM_game_process_display(); return;
    switch (ingame__DisplayMode)
    {
    case 1:
        // No action
        break;
    case 50:
        PlayCDTrack(game_music_track);
        if ( !(flags_general_unkn01 & 0x20) || !(gameturn & 0xF) )
        {
            gproc3_unknsub1();
            if ( flags_general_unkn01 & 0x800 )
              gproc3_unknsub2();
            BAT_play();
            if ( execute_commands )
            {
                long tmp;
                gamep_unknval_16 = gamep_unknval_13;
                gamep_unknval_13 = 0;
                ++gamep_unknval_12;
                gamep_unknval_10 += gamep_unknval_16;
                gamep_unknval_15 = gamep_unknval_14;
                tmp = gamep_unknval_14 + gamep_unknval_11;
                gamep_unknval_14 = 0;
                gamep_unknval_11 = tmp;
            }
        }
        break;
    case 55:
        show_menu_screen();
        break;
    case 58:
        gproc3_unknsub3(0);
        break;
    default:
        ERRORLOG("DisplayMode %d empty\n", (int)ingame__DisplayMode);
        break;
    }
}

void load_packet(void);
void game_process_sub02(void);
void process_packets(void);
void joy_input(void);
void game_process_sub04(void);
void process_packets(void);

void game_process(void)
{
    debug_m_sprite(193);
    DEBUGLOG(0,"WSCREEN 0x%lx", (ulong)lbDisplay.WScreen);
    while ( !exit_game )
    {
      process_sound_heap();
      if ( lbKeyOn[KC_F8] && lbShift & KMod_CONTROL )
      {
          lbKeyOn[KC_F8] = 0;
          LbScreenSetup(lbDisplay.ScreenMode, 640, 480, display_palette);
      }
      navi2_unkn_counter -= 2;
      if (navi2_unkn_counter < 0)
          navi2_unkn_counter = 0;
      if (navi2_unkn_counter > navi2_unkn_counter_max)
          navi2_unkn_counter_max = navi2_unkn_counter;
      if (cmdln_param_d)
          input_char = LbKeyboard();
      if (ingame__DisplayMode == 55)
          DEBUGLOG(0,"id=%d  trial alloc = %d turn %lu", 0, triangulation, gameturn);
      input();
      game_process_sub01();
      game_process_display();
      game_setup_sub4(gameturn + 100);
      load_packet();
      if ( ((active_flags_general_unkn01 & 0x8000) != 0) !=
        ((flags_general_unkn01 & 0x8000) != 0) )
          LbPaletteSet(display_palette);
      active_flags_general_unkn01 = flags_general_unkn01;
      if ( ingame__DisplayMode == 50 || ingame__DisplayMode == 1 || ingame__DisplayMode == 59 )
          game_process_sub02();
      if ( ingame__DisplayMode != 55 )
          process_packets();
      joy_input();
      if ( ingame__DisplayMode == 55 ) {
          swap_wscreen();
      }
      else if ( !(flags_general_unkn01 & 0x20) || ((gameturn & 0xF) == 0) ) {
          LbScreenSwapClear(0);
      }
      game_process_sub04();
      if ( unkn01_downcount > 0 ) /* orbital station explosion code */
      {
        unkn01_downcount--;
        DEBUGLOG(0,"unkn01_downcount = %ld", unkn01_downcount);
        if ( unkn01_downcount == 40 ) {
            mapwho_unkn01(unkn01_pos_x, unkn01_pos_y);
        }
        else if ( unkn01_downcount < 40 ) {
            unsigned short stl_y;
            unsigned short stl_x;
            stl_y = unkn01_pos_y + (LbRandomAnyShort() & 0xF) - 7;
            stl_x = unkn01_pos_x + (LbRandomAnyShort() & 0xF) - 7;
            new_bang_3(stl_x << 16, 0, stl_y << 16, 95);
            stl_y = unkn01_pos_y + (LbRandomAnyShort() & 0xF) - 7;
            stl_x = unkn01_pos_x + (LbRandomAnyShort() & 0xF) - 7;
            new_bang_3(stl_x << 16, 0, stl_y << 16, 95);
        }
      }
      gameturn++;
      game_process_sub09();
    }
    PacketRecord_Close();
    LbPaletteFade(NULL, 0x10u, 1);
}

void
game_quit(void)
{
    sound_finalise ();
    display_finalise ();
    SDL_Quit ();
    exit (0);
}

void
game_transform_path_full(const char *file_name, char *buffer, size_t size)
{
    if (strncasecmp (file_name, SAVEGAME_PATH,
             sizeof (SAVEGAME_PATH) - 1) == 0)
    {
        snprintf (buffer, size, "%s" FS_SEP_STR "%s", GetDirectoryUser(),
          file_name + sizeof (SAVEGAME_PATH) - 1);
        return;
    }

    /* abort on absolute paths */
    if (file_name[0] == '\\' || file_name[0] == '/'
        || (strlen (file_name) >= 2 && file_name[1] == ':'))
    {
        snprintf (buffer, size, "%s", file_name);
        return;
    }

    snprintf (buffer, size, "%s" FS_SEP_STR "%s", GetDirectoryHdd(), file_name);
}

void
game_transform_path(const char *file_name, char *result)
{
    game_transform_path_full (file_name, result, FILENAME_MAX);
}

void
game_play_music(void)
{
    char file_name[FILENAME_MAX];

    snprintf (file_name, sizeof (file_name),
          "%s" FS_SEP_STR "music" FS_SEP_STR "track_%i.ogg",
          GetDirectoryHdd(), game_music_track - 1);

    sound_open_music_file (file_name);
    sound_play_music ();
}

static void
game_update_full(bool wait)
{
    const int max_fps = 16;
    const int32_t frame_duration = 1000 / max_fps;

    static int32_t last_frame_ticks;
    int32_t start_ticks;

    display_unlock();

    sound_update();
    display_update();
    game_handle_sdl_events();

    start_ticks = SDL_GetTicks();

    if (wait && last_frame_ticks != 0)
    {
        int32_t last_frame_duration;

        last_frame_duration = (int32_t)(start_ticks - last_frame_ticks + 2);

        if (last_frame_duration < frame_duration)
        {
            int32_t total_sleep_time = frame_duration - last_frame_duration;
            const int32_t min_sleep_time = 1000 / 40;
            const int32_t max_sleep_time = 1000 / 20;

            total_sleep_time = frame_duration - last_frame_duration;

            if (total_sleep_time > 0)
            {
                float f = (float) total_sleep_time
                    * (min_sleep_time + max_sleep_time)
                    / (2 * min_sleep_time * max_sleep_time);
                int32_t base_sleep_time = (int32_t)(total_sleep_time / f + .5f);

                while (total_sleep_time > 0)
                {
                    int32_t sleep_time = MIN(base_sleep_time, total_sleep_time);
                    int32_t ticks = SDL_GetTicks();

                    SDL_Delay(sleep_time);

                    display_lock();
                    game_handle_sdl_events();
                    sound_update();
                    display_unlock();

                    display_update_mouse_pointer();

                    total_sleep_time -= SDL_GetTicks() - ticks;
                }
            }
        }
    }

    last_frame_ticks = SDL_GetTicks();

    display_lock();
}

int
game_wait_for_vsync(void)
{
    game_update_full(false);
    return 1;
}

void
game_update(void)
{
    game_update_full(true);
}

void
game_reset(void)
{
    host_reset();
    free_texturemaps();
    LbDataFreeAll(unk02_load_files);
}

