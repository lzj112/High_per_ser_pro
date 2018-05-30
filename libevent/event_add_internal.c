static inline int event_add_internal(struct event* ev, const struct timeval* tv,
                                    int tv_is_absolute) 
{
    struct event_base* base = ev->ev_base;
    int res = 0;
    int notify = 0;

    EVENT_BASE_ASSERT_LOCKED(base);
    _event_debug_assert_is_setup(ev);

    event_debug((
            "event_add: event: %p (fd %d), %s%s%scall %p",
            ev,
            (int)ev->ev_fd,
            ev->ev_events & EV_READ ? "EV_READ " : " ".
            ev->ev->events & EV_WRITE ? "EV_WRITE " : " ",
            tv ? "EV_TIMEOUT " : " ",
            ev->ev_callback));
    
    EVUTIL_ASSERT(!(ev->ev_flag & ~EVLIST_ALL));
    ...
}