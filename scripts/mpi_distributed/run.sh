#!/bin/bash 

if [ $# -le 1 ]; then
echo "Error: faltan parametros"
exit
fi

# Defaults
SLURM="no"
FILENAME=$0

EDDL=$HOME/git/eddl
BUILD=$EDDL/build
BIN=$BUILD/bin
SCRIPTS=$EDDL/scripts/mpi_distributed
DATASETS=~/convert_EDDL


NAME="run"

# process arguments
while  [ $# -ge 2 ]
do
	case $1 in 
        --slurm) SLURM="yes"  ;;
        --mpi) MPI="yes"  ;;
		-n) PROCS=$2 ; shift ;;
		--name) NAME=$2 ; shift ;;
		*) break ;;
	esac
	shift
done

CMDLINE=$*
#echo "SLURM" ${SLURM}

# Filenames
NAME_COMMON=${NAME}_n${PROCS}

NAME=nccl_${NAME_COMMON}

# Patches for mpi
if [[ "$MPI" == "yes" ]]; then
NAME=mpi_${NAME_COMMON}
fi


OUTPUT=$NAME.out
ERR=$NAME.err
EDDL_EXEC="${CMDLINE}"

# Patches for mpi
if [[ "$MPI" == "yes" ]]; then
EDDL_EXEC="$EDDL_EXEC --mpi"
fi

#MPI_PARAM="--report-bindings -map-by node:PE=28 --mca btl openib,self,vader --mca btl_openib_allow_ib true --mca mpi_leave_pinned 1"
#MPI_PARAM="--report-bindings -map-by node:PE=28 --mca btl openib,self,vader --mca btl_openib_allow_ib true"
#MPI_PARAM="--report-bindings -map-by node:PE=28 --mca pml ucx "

# Common MPI and sbatch commands
MPI_COMMON="--report-bindings"

SBATCH="sbatch -n ${PROCS} -N ${PROCS} --out ${OUTPUT} --err ${ERR} -J ${FILENAME} --exclusive"

# node specific options
if [[ "$HOSTNAME" =~ "altec" ]]; then
MPI_MACHINE="-map-by node:PE=14 --mca pml ucx --mca btl ^openib --mca btl_openib_verbose 1 --mca pml_ucx_verbose 1"
fi

if [[ "$HOSTNAME" =~ "cmts" ]]; then
MPI_MACHINE="-map-by node:PE=32 --mca btl_tcp_if_include ib1 --mca btl_openib_verbose 1 --mca pml_ucx_verbose 1 "
SBATCH="${SBATCH} --gres=gpu:1"
fi

if [[ "$HOSTNAME" =~ "p9login" ]]; then
MPI_MACHINE="--mca btl_openib_verbose 1 --mca pml_ucx_verbose 1"
SBATCH="${SBATCH} --gres=gpu:1 --cpus-per-task=40"
fi


# MPI command line parameters
MPI_PARAM="$MPI_MACHINE $MPI_COMMON"


## Run if file does not exists
if [ ! -f $OUTPUT ]
then


# Patches for slurm
if [[ "$SLURM" == "yes" ]]; then

COMMAND="mpirun $MPI_PARAM ${EDDL_EXEC}"

# Generate slurm sbatch file
echo "#!/bin/bash
#
$COMMAND" > $FILENAME.sbatch
############################

echo $SBATCH $FILENAME.sbatch
$SBATCH $FILENAME.sbatch

else
# Execute without slurm to stdout

COMMAND="mpirun -np $PROCS $MPI_PARAM -hostfile $SCRIPTS/cluster.nodos ${EDDL_EXEC}"
echo $COMMAND
echo $COMMAND >$ERR
$COMMAND >$OUTPUT 2>>$ERR

fi

else

echo "File $OUTPUT already exists. Bye"

fi
