
#ifndef COMMON_NEON_H
#define COMMON_NEON_H

#include <stdint.h>

#define _AA64_FLUSH_ZERO_MASK (1<<24)
#define _AA64_FLUSH_ZERO_ON   (1<<24)
#define _AA64_FLUSH_ZERO_OFF  (0<<24)

void _AA64_SET_FLUSH_ZERO_MODE (uint64_t _mode)
{

  _mode = (_mode<<24);

  asm volatile ( "mrs x9, fpcr" );
  asm volatile ( "mov x10, %[value]" : : [value] "r" (_mode) );
  asm volatile ( "bfi x9, x10, #24, #1" );
  asm volatile ( "msr fpcr, x9" );

}

uint64_t _AA64_GET_FLUSH_ZERO_MODE ( )
{

  uint64_t _mode;

  asm volatile ( "mrs %[value], fpcr" : [value] "=r" (_mode) : :);
  _mode = (_mode) & _AA64_FLUSH_ZERO_ON;

  return _mode;

}

/*
 * Determine if AdvSIMD/NEON is supported. Returns true if supported.
 */
inline
bool is_neon_supported()
{
  uint64_t neon_mask = 0b0000;
  uint64_t __val;
  uint64_t __res;

  // read ID_AA64PFR0_EL1
  asm volatile ( "mrs %[val], ID_AA64PFR0_EL1" : [val] "=r" (__val) );

  // get ID_AA64PFR0_EL.AdvSIMD, bits [23:20]
  __res = ((__val>>20) & ((1<<4)-1));

  return (__res == neon_mask);
  
}

/*
 * Determine if SVE is supported. Returns true if supported.
 */
inline
bool is_sve_supported()
{
  uint64_t sve_mask = 0b0001;
  uint64_t __val;
  uint64_t __res;

  // read ID_AA64PFR0_EL1
  asm volatile ( "mrs %[val], ID_AA64PFR0_EL1" : [val] "=r" (__val) );

  // get ID_AA64PFR0_EL.SVE, bits [35:32]
  __res = ((__val>>32) & ((1<<4)-1));

  return (__res == sve_mask);

}

#endif
