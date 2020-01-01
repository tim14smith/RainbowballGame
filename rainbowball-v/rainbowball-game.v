module main

import time
import gx
import gg
import glfw
import freetype

const (
	GlfwPressAction = 1
	GlfwReleaseAction = 0
	WinSize = 800
	TextSize = 12
	TimerPeriod = 15
	text_cfg = gx.TextCfg{
		align: gx.ALIGN_LEFT
		size: TextSize
		color: gx.rgb(0, 0, 0)
	}
	over_cfg = gx.TextCfg{
		align: gx.ALIGN_LEFT
		size: TextSize
		color: gx.White
	}
	Colors = [gx.rgb(255, 242, 0), // yellow
	gx.rgb(174, 0, 255), // purple
	gx.rgb(33, 66, 255), // blue
	gx.rgb(74, 198, 255), // lightblue
	gx.rgb(60, 255, 0), // green
	gx.rgb(255, 0, 0), // red
	gx.rgb(255, 180, 31) // orange
	]
	BackgroundColor = gx.White
	UIColor = gx.Red
)

enum GameState {
	paused
	running
	gameover
}

struct Game {
mut:
// State of the current game
	state         GameState
	// Position of the ball
	pos_x         int
	pos_y         int
	current_color int
	up_pressed    bool
	down_pressed  bool
	left_pressed  bool
	right_pressed bool
	frames_moved  int
	// gg context for drawing
	gg            &gg.GG
	// ft context for font drawing
	ft            &freetype.FreeType
	font_loaded   bool
}

fn main() {
	glfw.init_glfw()
	mut game := &Game{
		gg: gg.new_context(gg.Cfg{
			width: WinSize
			height: WinSize
			use_ortho: true // This is needed for 2D drawing
			
			create_window: true
			window_title: 'V Rainbowball'
			window_user_ptr: 0
		})
		ft: 0
		current_color: 0
		pos_x: 0
		pos_y: 0
	}
	game.init_game()
	game.gg.window.onkeydown(key_down)
	go game.run()
	game.gg.window.set_user_ptr(game)
	gg.clear(BackgroundColor)
	// Try to load font
	game.ft = freetype.new_context(gg.Cfg{
		width: WinSize
		height: WinSize
		use_ortho: true
		font_size: 18
		scale: 2
		window_user_ptr: 0
	})
	game.font_loaded = (game.ft != 0)
	for {
		gg.clear(BackgroundColor)
		game.draw_scene()
		game.gg.render()
		if game.gg.window.should_close() {
			game.gg.window.destroy()
			return
		}
	}
}

fn (g mut Game) init_game() {
	g.state = .running
}

fn (g mut Game) run() {
	for {
		if g.state == .running {
			g.state = .running
		}
		glfw.post_empty_event() // force window redraw
		time.sleep_ms(TimerPeriod)
	}
}

fn (g mut Game) draw_ui() {
	if g.font_loaded {
		g.ft.draw_text(100, 100, 'Arrow keys to move. Esc to quit.', text_cfg)
		if g.state == .gameover {
			g.gg.draw_rect(0, WinSize / 2 - TextSize, WinSize, 5 * TextSize, UIColor)
			g.ft.draw_text(1, WinSize / 2 + 0 * TextSize, 'Game Over', over_cfg)
			g.ft.draw_text(1, WinSize / 2 + 2 * TextSize, 'Space to restart', over_cfg)
		}
		else if g.state == .paused {
			g.gg.draw_rect(0, WinSize / 2 - TextSize, WinSize, 5 * TextSize, UIColor)
			g.ft.draw_text(1, WinSize / 2 + 0 * TextSize, 'Game Paused', text_cfg)
			g.ft.draw_text(1, WinSize / 2 + 2 * TextSize, 'SPACE to resume', text_cfg)
		}
	}
	// g.gg.draw_rect(0, BlockSize, WinSize, LimitThickness, UIColor)
}

fn (g mut Game) draw_scene() {
	g.draw_ui()
	mut v_ud, mut v_lr := g.get_velocity()
	g.pos_x = g.pos_x + v_lr * 5
	if g.pos_x <= 0 {
		g.pos_x = 0
		v_lr = 0
	} else if g.pos_x >= (WinSize - 30) {
		g.pos_x = WinSize - 30
		v_lr = 0
	}
	g.pos_y = g.pos_y + v_ud * 5
	if g.pos_y <= 0 {
		g.pos_y = 0
		v_ud = 0
	} else if g.pos_y >= (WinSize - 30) {
		g.pos_y = WinSize - 30
		v_ud = 0
	}
	if v_ud != 0 || v_lr != 0 {
		g.frames_moved++
		if g.frames_moved > 4 {
			g.frames_moved = 0
			g.next_color()
		}
	}
	g.gg.draw_rect(g.pos_x, g.pos_y, 30, 30, Colors[g.current_color])
}

// Get velocity left/right and up/down. Neg means left/up, Pos means right/down
fn (g Game) get_velocity() (int,int) {
	up := if g.up_pressed { -1 } else { 0 }
	down := if g.down_pressed { 1 } else { 0 }
	left := if g.left_pressed { -1 } else { 0 }
	right := if g.right_pressed { 1 } else { 0 }
	return (up + down),(left + right)
}

fn (g mut Game) next_color() {
	g.current_color = (g.current_color + 1) % Colors.len
}

// TODO: this exposes the unsafe C interface, clean up
fn key_down(wnd voidptr, key, code, action, mods int) {
	if action != GlfwPressAction && action != GlfwReleaseAction {
		return
	}
	// Fetch the game object stored in the user pointer
	mut game := &Game(glfw.get_window_user_pointer(wnd))
	// global keys
	if action == GlfwPressAction {
		match key {
			glfw.KEY_ESCAPE {
				glfw.set_should_close(wnd, true)
			}
			glfw.key_space {
				if game.state == .running {
					game.state = .paused
				}
				else if game.state == .paused {
					game.state = .running
				}
				else if game.state == .gameover {
					game.init_game()
					game.state = .running
				}
			}
			else {}
	}
	}
	if game.state != .running {
		return
	}
	// keys while game is running
	pressed := action == GlfwPressAction
	match key {
		glfw.KeyUp {
			game.up_pressed = pressed
		}
		glfw.KeyLeft {
			game.left_pressed = pressed
		}
		glfw.KeyRight {
			game.right_pressed = pressed
		}
		glfw.KeyDown {
			game.down_pressed = pressed
		}
		else {}
	}
}
