// auto-generated [Fri Jun  6 23:25:35 2008] by ./maketypelist.pl
struct rule_list_0:rule_list_base < nil_t, nil_t >
{
};
struct char_list_0:char_list_base < -1, nil_t >
{
};

template < class T1 > struct rule_list_1:rule_list_base < T1, nil_t >
{
};
template < char_type V1 > struct char_list_1:char_list_base < V1, nil_t >
{
};

template < class T1, class T2 > struct rule_list_2:rule_list_base < T1, T2 >
{
};
template < char_type V1, char_type V2 >
  struct char_list_2:char_list_base < V1, char_list_base < V2, nil_t > >
{
};

template < class T1, class T2, class T3 >
  struct rule_list_3:rule_list_base < T1, rule_list_2 < T2, T3 > >
{
};

template < char_type V1, char_type V2, char_type V3 >
  struct char_list_3:char_list_base < V1, char_list_2 < V2, V3 > >
{
};

template < class T1, class T2, class T3, class T4 >
  struct rule_list_4:rule_list_base < T1, rule_list_3 < T2, T3, T4 > >
{
};

template < char_type V1, char_type V2, char_type V3, char_type V4 >
  struct char_list_4:char_list_base < V1, char_list_3 < V2, V3, V4 > >
{
};

template < class T1, class T2, class T3, class T4, class T5 >
  struct rule_list_5:rule_list_base < T1, rule_list_4 < T2, T3, T4, T5 > >
{
};

template < char_type V1, char_type V2, char_type V3, char_type V4,
  char_type V5 > struct char_list_5:char_list_base < V1, char_list_4 < V2, V3,
  V4, V5 > >
{
};

template < class T1, class T2, class T3, class T4, class T5, class T6 >
  struct rule_list_6:rule_list_base < T1, rule_list_5 < T2, T3, T4, T5, T6 > >
{
};

template < char_type V1, char_type V2, char_type V3, char_type V4,
  char_type V5, char_type V6 > struct char_list_6:char_list_base < V1,
  char_list_5 < V2, V3, V4, V5, V6 > >
{
};

template < class T1, class T2, class T3, class T4, class T5, class T6,
  class T7 > struct rule_list_7:rule_list_base < T1, rule_list_6 < T2, T3, T4,
  T5, T6, T7 > >
{
};

template < char_type V1, char_type V2, char_type V3, char_type V4,
  char_type V5, char_type V6,
  char_type V7 > struct char_list_7:char_list_base < V1, char_list_6 < V2, V3,
  V4, V5, V6, V7 > >
{
};

template < class T1, class T2, class T3, class T4, class T5, class T6,
  class T7, class T8 > struct rule_list_8:rule_list_base < T1,
  rule_list_7 < T2, T3, T4, T5, T6, T7, T8 > >
{
};

template < char_type V1, char_type V2, char_type V3, char_type V4,
  char_type V5, char_type V6, char_type V7,
  char_type V8 > struct char_list_8:char_list_base < V1, char_list_7 < V2, V3,
  V4, V5, V6, V7, V8 > >
{
};

template < class T1, class T2, class T3, class T4, class T5, class T6,
  class T7, class T8, class T9 > struct rule_list_9:rule_list_base < T1,
  rule_list_8 < T2, T3, T4, T5, T6, T7, T8, T9 > >
{
};

template < char_type V1, char_type V2, char_type V3, char_type V4,
  char_type V5, char_type V6, char_type V7, char_type V8,
  char_type V9 > struct char_list_9:char_list_base < V1, char_list_8 < V2, V3,
  V4, V5, V6, V7, V8, V9 > >
{
};

template < class T1, class T2, class T3, class T4, class T5, class T6,
  class T7, class T8, class T9,
  class T10 > struct rule_list_10:rule_list_base < T1, rule_list_9 < T2, T3,
  T4, T5, T6, T7, T8, T9, T10 > >
{
};

template < char_type V1, char_type V2, char_type V3, char_type V4,
  char_type V5, char_type V6, char_type V7, char_type V8, char_type V9,
  char_type V10 > struct char_list_10:char_list_base < V1, char_list_9 < V2,
  V3, V4, V5, V6, V7, V8, V9, V10 > >
{
};

template < class T1 = nil_t, class T2 = nil_t, class T3 = nil_t, class T4 =
  nil_t, class T5 = nil_t, class T6 = nil_t, class T7 = nil_t, class T8 =
  nil_t, class T9 = nil_t, class T10 =
  nil_t > struct rule_list:rule_list_10 < T1, T2, T3, T4, T5, T6, T7, T8, T9,
  T10 >
{
};
template < char_type V1 = -1, char_type V2 = -1, char_type V3 =
  -1, char_type V4 = -1, char_type V5 = -1, char_type V6 = -1, char_type V7 =
  -1, char_type V8 = -1, char_type V9 = -1, char_type V10 =
  -1 > struct char_list:char_list_10 < V1, V2, V3, V4, V5, V6, V7, V8, V9,
  V10 >
{
};

template < class T1, class T2, class T3, class T4, class T5, class T6,
  class T7, class T8, class T9 > struct rule_list <T1, T2, T3, T4, T5, T6, T7,
  T8, T9, nil_t >:rule_list_9 < T1, T2, T3, T4, T5, T6, T7, T8, T9 >
{
};
template < char_type V1, char_type V2, char_type V3, char_type V4,
  char_type V5, char_type V6, char_type V7, char_type V8,
  char_type V9 > struct char_list <V1, V2, V3, V4, V5, V6, V7, V8, V9,
  -1 >:char_list_9 < V1, V2, V3, V4, V5, V6, V7, V8, V9 >
{
};

template < class T1, class T2, class T3, class T4, class T5, class T6,
  class T7, class T8 > struct rule_list <T1, T2, T3, T4, T5, T6, T7, T8,
  nil_t, nil_t >:rule_list_8 < T1, T2, T3, T4, T5, T6, T7, T8 >
{
};
template < char_type V1, char_type V2, char_type V3, char_type V4,
  char_type V5, char_type V6, char_type V7,
  char_type V8 > struct char_list <V1, V2, V3, V4, V5, V6, V7, V8, -1,
  -1 >:char_list_8 < V1, V2, V3, V4, V5, V6, V7, V8 >
{
};

template < class T1, class T2, class T3, class T4, class T5, class T6,
  class T7 > struct rule_list <T1, T2, T3, T4, T5, T6, T7, nil_t, nil_t,
  nil_t >:rule_list_7 < T1, T2, T3, T4, T5, T6, T7 >
{
};
template < char_type V1, char_type V2, char_type V3, char_type V4,
  char_type V5, char_type V6, char_type V7 > struct char_list <V1, V2, V3, V4,
  V5, V6, V7, -1, -1, -1 >:char_list_7 < V1, V2, V3, V4, V5, V6, V7 >
{
};

template < class T1, class T2, class T3, class T4, class T5, class T6 >
  struct rule_list <T1, T2, T3, T4, T5, T6, nil_t, nil_t, nil_t,
  nil_t >:rule_list_6 < T1, T2, T3, T4, T5, T6 >
{
};
template < char_type V1, char_type V2, char_type V3, char_type V4,
  char_type V5, char_type V6 > struct char_list <V1, V2, V3, V4, V5, V6, -1,
  -1, -1, -1 >:char_list_6 < V1, V2, V3, V4, V5, V6 >
{
};

template < class T1, class T2, class T3, class T4, class T5 >
  struct rule_list <T1, T2, T3, T4, T5, nil_t, nil_t, nil_t, nil_t,
  nil_t >:rule_list_5 < T1, T2, T3, T4, T5 >
{
};
template < char_type V1, char_type V2, char_type V3, char_type V4,
  char_type V5 > struct char_list <V1, V2, V3, V4, V5, -1, -1, -1, -1,
  -1 >:char_list_5 < V1, V2, V3, V4, V5 >
{
};

template < class T1, class T2, class T3, class T4 >
  struct rule_list <T1, T2, T3, T4, nil_t, nil_t, nil_t, nil_t, nil_t,
  nil_t >:rule_list_4 < T1, T2, T3, T4 >
{
};
template < char_type V1, char_type V2, char_type V3, char_type V4 >
  struct char_list <V1, V2, V3, V4, -1, -1, -1, -1, -1, -1 >:char_list_4 < V1,
  V2, V3, V4 >
{
};

template < class T1, class T2, class T3 >
  struct rule_list <T1, T2, T3, nil_t, nil_t, nil_t, nil_t, nil_t, nil_t,
  nil_t >:rule_list_3 < T1, T2, T3 >
{
};
template < char_type V1, char_type V2, char_type V3 >
  struct char_list <V1, V2, V3, -1, -1, -1, -1, -1, -1, -1 >:char_list_3 < V1,
  V2, V3 >
{
};

template < class T1, class T2 >
  struct rule_list <T1, T2, nil_t, nil_t, nil_t, nil_t, nil_t, nil_t, nil_t,
  nil_t >:rule_list_base < T1, T2 >
{
};

template < char_type V1, char_type V2 >
  struct char_list <V1, V2, -1, -1, -1, -1, -1, -1, -1,
  -1 >:char_list_base < V1, char_list_base < V2, nil_t > >
{
};

template < class T1 >
  struct rule_list <T1, nil_t, nil_t, nil_t, nil_t, nil_t, nil_t, nil_t,
  nil_t, nil_t >:rule_list_base < T1, nil_t >
{
};

template < char_type V1 >
  struct char_list <V1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1 >:char_list_base < V1, nil_t >
{
};

template <> struct rule_list <>:rule_list_base < nil_t, nil_t >
{
};

template <> struct char_list <>:char_list_base < -1, nil_t >
{
};
