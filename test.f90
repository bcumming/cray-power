!==============================================================================
program energytest
!==============================================================================

use omp_lib

implicit none

integer :: n, i, num_runs, ierr, run
real (kind=8), allocatable :: a(:), b(:), c(:)

real (kind=8)   :: ssum, timespent
real (kind=8)   :: energy_init, energy_final
real (kind=8)   :: device_energy_init, device_energy_final

n=100000000
num_runs=100

! allocate working memory
allocate(a(n),  b(n), c(n), stat=ierr)

write (*,*) omp_get_max_threads(), ' threads'

! compute cumulative energy
call error(ierr /= 0, 'Problem allocating memory')

! initialize
do i=1,n
    a(i) = 1.;
    b(i) = 1.;
    c(i) = 1.;
enddo

! get energy counter at startup
call energy(energy_init)
call device_energy(device_energy_init)

timespent = -omp_get_wtime()

!$omp parallel
do run=1,num_runs
!$omp do
    do i=1,n
        a(i) = a(i) + b(i)*c(i)
    enddo
!$omp end do
enddo
!$omp end parallel

ssum=0.
!$omp parallel do reduction(+:ssum)
do i=1,n
    ssum = ssum+a(i)
enddo
!$omp end parallel do

timespent = timespent + omp_get_wtime()

! get energy counter at end
call energy(energy_final)
call device_energy(device_energy_final)

write (*,*) 'that took ', energy_final-energy_init, ' Joules'
write (*,*) 'at rate ', (energy_final-energy_init)/timespent, ' Watts'
write (*,*) 'that took ', timespent, ' seconds'

deallocate(a, b, c)
!==============================================================================
contains
!==============================================================================

subroutine device_energy(e)
    implicit none
    real (kind=8), intent(out) :: e

    open(unit=50, file='/sys/cray/pm_counters/accel_energy' ,action='READ')
    read(50,*) e
    close(50)
end

subroutine energy(e)
    implicit none
    real (kind=8), intent(out) :: e

    open(unit=50, file='/sys/cray/pm_counters/energy' ,action='READ')
    read(50,*) e
    close(50)
end

subroutine device_power(p)
    implicit none
    real (kind=8), intent(out) :: p

    open(unit=50, file='/sys/cray/pm_counters/accel_power' ,action='READ')
    read(50,*) p
    close(50)
end

subroutine power(p)
    implicit none
    real (kind=8), intent(out) :: p

    open(unit=50, file='/sys/cray/pm_counters/power' ,action='READ')
    read(50,*) p
    close(50)
end

! write error message and terminate
subroutine error(yes, msg)
    implicit none

    ! arguments
    logical, intent(in) :: yes
    character(len=*), intent(in) :: msg

    ! local
    integer, external :: lnblnk

    if (yes) then
        write(0,*) 'FATAL PROGRAM ERROR!'
        write(0,*) msg
        write(0,*) 'Execution aborted...'
        stop
    end if
end
!==============================================================================
end program energytest
!==============================================================================
