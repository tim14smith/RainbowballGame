import rand
import time
import gx
import gg
import glfw
import math
import freetype

const (
	WinWidth = 640
	WinHeight = 480
	TextSize = 12
	TimerPeriod = 250
	text_cfg = gx.TextCfg{
		align:gx.ALIGN_LEFT
		size:TextSize
		color:gx.rgb(0, 0, 0)
	}
	over_cfg = gx.TextCfg{
		align:gx.ALIGN_LEFT
		size:TextSize
		color:gx.White
	}
	Colors = [
		gx.rgb(0, 0, 0),        // unused ?
		gx.rgb(255, 242, 0),    // yellow quad
		gx.rgb(174, 0, 255),    // purple triple
		gx.rgb(60, 255, 0),     // green short topright
		gx.rgb(255, 0, 0),      // red short topleft
		gx.rgb(255, 180, 31),   // orange long topleft
		gx.rgb(33, 66, 255),    // blue long topright
		gx.rgb(74, 198, 255),   // lightblue longest
		gx.rgb(0, 170, 170),    // unused ?
	]

	BackgroundColor = gx.White
	UIColor = gx.Red
)

enum GameState {
        paused running gameover
}

struct Game {
	mut:
	// State of the current game
	state    GameState
	// Position of the ball
	pos_x        int
	pos_y        int
	// gg context for drawing
	gg          &gg.GG
	// ft context for font drawing
	ft          &freetype.FreeType
	font_loaded bool
}


fn main() {
	glfw.init_glfw()
	mut game := &Game{
		gg: gg.new_context(gg.Cfg {
			width: WinWidth
			height: WinHeight
			use_ortho: true // This is needed for 2D drawing
			create_window: true
			window_title: 'V Rainbowball'
			window_user_ptr: game
		})
		ft: 0
	}
	game.init_game()
	game.gg.window.onkeydown(key_down)
	go game.run()
	game.gg.window.set_user_ptr(game)
	gg.clear(BackgroundColor)
	// Try to load font
	game.ft = freetype.new_context(gg.Cfg{
		width: WinWidth
		height: WinHeight
		use_ortho: true
		font_size: 18
		scale: 2
		window_user_ptr: 0
	})
	game.font_loaded = (game.ft != 0 )
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
		g.ft.draw_text(1, 3,'WOOOO', text_cfg)
		if g.state == .gameover {
			g.gg.draw_rect(0, WinHeight / 2 - TextSize, WinWidth,
		 								5 * TextSize, UIColor)
			g.ft.draw_text(1, WinHeight / 2 + 0 * TextSize, 'Game Over', over_cfg)
			g.ft.draw_text(1, WinHeight / 2 + 2 * TextSize, 'Space to restart', over_cfg)
		} else if g.state == .paused {
			g.gg.draw_rect(0, WinHeight / 2 - TextSize, WinWidth,
				5 * TextSize, UIColor)
			g.ft.draw_text(1, WinHeight / 2 + 0 * TextSize, 'Game Paused', text_cfg)
			g.ft.draw_text(1, WinHeight / 2 + 2 * TextSize, 'SPACE to resume', text_cfg)
		}
	}
	//g.gg.draw_rect(0, BlockSize, WinWidth, LimitThickness, UIColor)
}

fn (g mut Game) draw_scene() {
	//g.draw_tetro()
	//g.draw_field()
	g.draw_ui()
}


// TODO: this exposes the unsafe C interface, clean up
fn key_down(wnd voidptr, key, code, action, mods int) {
	if action != 2 && action != 1 {
		return
	}
	// Fetch the game object stored in the user pointer
	mut game := &Game(glfw.get_window_user_pointer(wnd))
	// global keys
	match key {
		glfw.KEY_ESCAPE {
			glfw.set_should_close(wnd, true)
		}
		glfw.key_space {
			if game.state == .running {
				game.state = .paused
			} else if game.state == .paused {
				game.state = .running
			} else if game.state == .gameover {
				game.init_game()
				game.state = .running
			}
		}
		else {}
	}

	if game.state != .running {
		return
	}
	// keys while game is running
	match key {
	glfw.KeyUp {
		println('up')
	}
	glfw.KeyLeft {
		println('left')
	}
	glfw.KeyRight {
		println('right')
	}
	glfw.KeyDown {
		println('down')
	}
	else { }
	}
}