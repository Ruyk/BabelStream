// Copyright (c) 2015-16 Tom Deakin, Simon McIntosh-Smith,
// University of Bristol HPC
//
// For full license terms please see the LICENSE file distributed with this
// source code


#include "KokkosStream.hpp"

template <class T>
KokkosStream<T>::KokkosStream(
        const unsigned int ARRAY_SIZE, const int device_index)
    : array_size(ARRAY_SIZE)
{
  Kokkos::initialize();

  d_a = new Kokkos::View<double*>("d_a", ARRAY_SIZE);
  d_b = new Kokkos::View<double*>("d_b", ARRAY_SIZE);
  d_c = new Kokkos::View<double*>("d_c", ARRAY_SIZE);
  hm_a = new Kokkos::View<double*>::HostMirror();
  hm_b = new Kokkos::View<double*>::HostMirror();
  hm_c = new Kokkos::View<double*>::HostMirror();
  *hm_a = create_mirror_view(*d_a);
  *hm_b = create_mirror_view(*d_b);
  *hm_c = create_mirror_view(*d_c);
}

template <class T>
KokkosStream<T>::~KokkosStream()
{
  Kokkos::finalize();
}

template <class T>
void KokkosStream<T>::init_arrays(T initA, T initB, T initC)
{
  Kokkos::View<double*> a(*d_a);
  Kokkos::View<double*> b(*d_b);
  Kokkos::View<double*> c(*d_c);
  Kokkos::parallel_for(array_size, KOKKOS_LAMBDA (const long index)
  {
    a[index] = initA;
    b[index] = initB;
    c[index] = initC;
  });
  Kokkos::fence();
}

template <class T>
void KokkosStream<T>::read_arrays(
        std::vector<T>& a, std::vector<T>& b, std::vector<T>& c)
{
  deep_copy(*hm_a, *d_a);
  deep_copy(*hm_b, *d_b);
  deep_copy(*hm_c, *d_c);
  for(int ii = 0; ii < array_size; ++ii)
  {
    a[ii] = (*hm_a)(ii);
    b[ii] = (*hm_b)(ii);
    c[ii] = (*hm_c)(ii);
  }
}

template <class T>
void KokkosStream<T>::copy()
{
  Kokkos::View<double*> a(*d_a);
  Kokkos::View<double*> b(*d_b);
  Kokkos::View<double*> c(*d_c);

  Kokkos::parallel_for(array_size, KOKKOS_LAMBDA (const long index)
  {
    c[index] = a[index];
  });
  Kokkos::fence();
}

template <class T>
void KokkosStream<T>::mul()
{
  Kokkos::View<double*> a(*d_a);
  Kokkos::View<double*> b(*d_b);
  Kokkos::View<double*> c(*d_c);

  const T scalar = startScalar;
  Kokkos::parallel_for(array_size, KOKKOS_LAMBDA (const long index)
  {
    b[index] = scalar*c[index];
  });
  Kokkos::fence();
}

template <class T>
void KokkosStream<T>::add()
{
  Kokkos::View<double*> a(*d_a);
  Kokkos::View<double*> b(*d_b);
  Kokkos::View<double*> c(*d_c);

  Kokkos::parallel_for(array_size, KOKKOS_LAMBDA (const long index)
  {
    c[index] = a[index] + b[index];
  });
  Kokkos::fence();
}

template <class T>
void KokkosStream<T>::triad()
{
  Kokkos::View<double*> a(*d_a);
  Kokkos::View<double*> b(*d_b);
  Kokkos::View<double*> c(*d_c);

  const T scalar = startScalar;
  Kokkos::parallel_for(array_size, KOKKOS_LAMBDA (const long index)
  {
    a[index] = b[index] + scalar*c[index];
  });
  Kokkos::fence();
}

template <class T>
T KokkosStream<T>::dot()
{
  Kokkos::View<double *> a(*d_a);
  Kokkos::View<double *> b(*d_b);

  T sum = 0.0;

  Kokkos::parallel_reduce(array_size, KOKKOS_LAMBDA (const long index, double &tmp)
  {
    tmp += a[index] * b[index];
  }, sum);

  return sum;

}

void listDevices(void)
{
  std::cout << "This is not the device you are looking for.";
}


std::string getDeviceName(const int device)
{
  return "Kokkos";
}


std::string getDeviceDriver(const int device)
{
  return "Kokkos";
}

//template class KokkosStream<float>;
template class KokkosStream<double>;