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

template < class T1 = nil_t, class T2 = nil_t, class T3 = nil_t, class T4 =
  nil_t, class T5 =
  nil_t > struct rule_list:rule_list_05 < T1, T2, T3, T4, T5 >
{
};
template < char_type V1 = -1, char_type V2 = -1, char_type V3 =
  -1, char_type V4 = -1, char_type V5 =
  -1 > struct char_list:char_list_05 < V1, V2, V3, V4, V5 >
{
};

template < class T1, class T2, class T3, class T4 >
  struct rule_list <T1, T2, T3, T4, nil_t >:rule_list_4 < T1, T2, T3, T4 >
{
};
template < char_type V1, char_type V2, char_type V3, char_type V4 >
  struct char_list <V1, V2, V3, V4, -1 >:char_list_4 < V1, V2, V3, V4 >
{
};

template < class T1, class T2, class T3 >
  struct rule_list <T1, T2, T3, nil_t, nil_t >:rule_list_3 < T1, T2, T3 >
{
};
template < char_type V1, char_type V2, char_type V3 >
  struct char_list <V1, V2, V3, -1, -1 >:char_list_3 < V1, V2, V3 >
{
};

template < class T1, class T2 >
  struct rule_list <T1, T2, nil_t, nil_t, nil_t >:rule_list_base < T1, T2 >
{
};

template < char_type V1, char_type V2 >
  struct char_list <V1, V2, -1, -1, -1 >:char_list_base < V1,
  char_list_base < V2, nil_t > >
{
};

template < class T1 >
  struct rule_list <T1, nil_t, nil_t, nil_t, nil_t >:rule_list_base < T1,
  nil_t >
{
};

template < char_type V1 >
  struct char_list <V1, -1, -1, -1, -1 >:char_list_base < V1, nil_t >
{
};

template <> struct rule_list <>:rule_list_base < nil_t, nil_t >
{
};

template <> struct char_list <>:char_list_base < -1, nil_t >
{
};
