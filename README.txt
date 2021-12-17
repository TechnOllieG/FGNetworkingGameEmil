Hello Emil!

So this is what I've made:
- I refactored all X/Y floats to use Vector2's instead (just wrote a quick Vector2 struct).
- I added sprinting (with LShift) that is client activated (for this I also added some more entries to the Key enum to get that scancode).
- I added pushback when players move into each other as well as when you shoot someone with a projectile. (I guess this is
kinda server activated, but added it mostly for fun).
- I added points that are randomly spawned on the map (this is my main server activated mechanic).
These points also act as lives, so the player will only get kicked when all points are expended.
- I added a scoreboard for the points that is synced, also when new players join (even though all points should technically be 0 when new people join).