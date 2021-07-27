template <class T1, class T2>
class Either
{
    bool isLeft;
    union
    {
      T1 left;
      T2 right;
    };
    template<class T1_, class T2_> friend Either<T1_, T2_> Left(T1_ x);
    template<class T1_, class T2_> friend Either<T1_, T2_> Right(T2_ x);
  public:
    bool matchLeft(T1& x)
    {
      if (isLeft)
        x = left;
      return isLeft;
    }
    bool matchRight(T2& x)
    {
      if (!isLeft)
        x = right;
      return !isLeft;
    }
};

template <class T1, class T2>
Either<T1, T2> Left(T1 x)
{
  Either<T1, T2> e;
  e.isLeft = true;
  e.left = x;
  return e;
}
template <class T1, class T2>
Either<T1, T2> Right(T2 x)
{
  Either<T1, T2> e;
  e.isLeft = false;
  e.right = x;
  return e;
}