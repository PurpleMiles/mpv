#include "common/common.h"
#include "stream/stream.h"
#include "demux.h"

#include "timeline.h"

struct timeline *timeline_load(struct mpv_global *global, struct mp_log *log,
                               struct demuxer *demuxer)
{
    if (!demuxer->desc->load_timeline)
        return NULL;

    struct timeline *tl = talloc_ptrtype(NULL, tl);
    *tl = (struct timeline){
        .global = global,
        .log = log,
        .demuxer = demuxer,
        .track_layout = demuxer,
    };

    demuxer->desc->load_timeline(tl);

    if (tl->num_parts)
        return tl;
    timeline_destroy(tl);
    return NULL;
}

void timeline_destroy(struct timeline *tl)
{
    if (!tl)
        return;
    for (int n = 0; n < tl->num_sources; n++) {
        struct demuxer *d = tl->sources[n];
        if (d != tl->demuxer) {
            struct stream *s = d->stream;
            free_demuxer(d);
            free_stream(s);
        }
    }
    talloc_free(tl);
}
