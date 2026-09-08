uint8 id
string size_class
# pending | in_progress | completed
string task_state
bool visible
int32 observation_index
# Historical/latest stable pixel observation. It is a current grasp observation
# only when visible=true and the parent observation_valid=true.
float32 last_u
float32 last_v
float32 last_radius_px
int64 last_seen_ms
bool position_valid
geometry_msgs/PointStamped position
