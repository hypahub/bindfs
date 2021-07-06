
#include "usermap.h"
#include "userinfo.h"
#include <stdlib.h>

struct UserMap {
    uid_t *user_uid_from;
    uid_t *user_uid_to;

    uid_t *group_uid_from;
    gid_t *group_gid_from;
    gid_t *group_gid_to;

    int user_capacity;
    int group_capacity;
    int user_size;
    int group_size;
};

UserMap *usermap_create()
{
    UserMap* map = (UserMap*)malloc(sizeof(UserMap));
    map->user_uid_from = NULL;
    map->user_uid_to = NULL;
    map->group_uid_from = NULL;
    map->user_capacity = 0;
    map->group_capacity = 0;
    map->user_size = 0;
    map->group_size = 0;
    return map;
}

void usermap_destroy(UserMap *map)
{
    free(map->user_uid_from);
    free(map->user_uid_to);
    free(map->group_uid_from);
    free(map->group_gid_from);
    free(map->group_gid_to);
    free(map);
}

UsermapStatus usermap_add_uid(UserMap *map, uid_t from, uid_t to)
{
    int i;
    if (from == to) {
        return usermap_status_ok;
    }
    if (map->user_size == map->user_capacity) {
        if (map->user_capacity == 0) {
            map->user_capacity = 8;
        } else {
            map->user_capacity *= 2;
        }
        map->user_uid_from = (uid_t*)realloc(map->user_uid_from, map->user_capacity * sizeof(uid_t));
        map->user_uid_to = (uid_t*)realloc(map->user_uid_to, map->user_capacity * sizeof(uid_t));
    }
    if (usermap_get_uid_or_default(map, from, -1) != -1) {
        return usermap_status_duplicate_key;
    }
    i = map->user_size;
    map->user_uid_from[i] = from;
    map->user_uid_to[i] = to;
    map->user_size += 1;
    return usermap_status_ok;
}

UsermapStatus usermap_add_gid(UserMap *map, uid_t uid_from, gid_t gid_from, gid_t gid_to)
{
    int i;
    if (gid_from == gid_to) {
        return usermap_status_ok;
    }
    if (map->group_size == map->group_capacity) {
        if (map->group_capacity == 0) {
            map->group_capacity = 8;
        } else {
            map->group_capacity *= 2;
        }
        map->group_uid_from = (uid_t*)realloc(map->group_uid_from, map->group_capacity * sizeof(uid_t));
        map->group_gid_from = (gid_t*)realloc(map->group_gid_from, map->group_capacity * sizeof(gid_t));
        map->group_gid_to = (gid_t*)realloc(map->group_gid_to, map->group_capacity * sizeof(gid_t));
    }
    if (usermap_get_gid_or_default(map, uid_from, gid_from, -1) != -1) {
        return usermap_status_duplicate_key;
    }
    i = map->group_size;
    map->group_uid_from[i] = uid_from;
    map->group_gid_from[i] = gid_from;
    map->group_gid_to[i] = gid_to;
    map->group_size += 1;
    return usermap_status_ok;
}

const char* usermap_errorstr(UsermapStatus status)
{
    switch (status) {
        case usermap_status_ok: return "ok";
        case usermap_status_duplicate_key: return "user mapped twice";
        default: return "unknown error";
    }
}

uid_t usermap_get_uid_or_default(UserMap *map, uid_t u, uid_t deflt)
{
    int i;
    for (i = 0; i < map->user_size; ++i) {
        if (map->user_uid_from[i] == u) {
            return map->user_uid_to[i];
        }
    }
    return deflt;
}

gid_t usermap_get_gid_or_default(UserMap *map, uid_t u, gid_t g, gid_t deflt)
{
    int i;
    for (i = 0; i < map->group_size; ++i) {
        if (map->group_gid_from[i] == g && (map->group_uid_from[i] == u || map->group_uid_from[i] == (gid_t)-1)) {
            return map->group_gid_to[i];
        }
    }
    return deflt;
}
