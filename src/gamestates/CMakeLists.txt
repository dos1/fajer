register_gamestate("loading")
register_gamestate("dosowisko")
register_gamestate("menu")
register_gamestate("game")
register_gamestate("exit")
register_gamestate("win")
register_gamestate("lose")

option(WITH_RUST "build gamestates written in Rust" false)

if(WITH_RUST)
  add_subdirectory("rust")
endif(WITH_RUST)
