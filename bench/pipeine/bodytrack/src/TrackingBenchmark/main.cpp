//-------------------------------------------------------------
//      ____                        _      _
//     / ___|____ _   _ ____   ____| |__  | |
//    | |   / ___| | | |  _  \/ ___|  _  \| |
//    | |___| |  | |_| | | | | |___| | | ||_|
//     \____|_|  \_____|_| |_|\____|_| |_|(_) Media benchmarks
//                         
//	  2006, Intel Corporation, licensed under Apache 2.0 
//
//  file : main.cpp
//  author : Scott Ettinger - scott.m.ettinger@intel.com
//  description : Top level body tracking code.  Takes image
//				  inputs from disk and runs the tracking
//				  particle filter.
//
//				  Currently contains 3 versions :
//				  Single threaded, OpenMP, and Posix threads.
//				  They are kept separate for readability.
//
//				  Thread methods supported are selected by the 
//				  #defines USE_OPENMP, USE_THREADS or USE_TBB. 
//
//  modified : 
//--------------------------------------------------------------

#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>

//Add defines USE_OPENMP, USE_THREADS or USE_TBB for threaded versions if not using config file (Windows).
//#define USE_OPENMP
//#define USE_THREADS
//#define USE_TBB

#if defined(USE_OPENMP)
#include <omp.h>
#include "ParticleFilterOMP.h"
#include "TrackingModelOMP.h"
#endif //USE_OPENMP

#if defined(USE_THREADS)
#include "threads/Thread.h"
#include "threads/WorkerGroup.h"
#include "ParticleFilterPthread.h"
#include "TrackingModelPthread.h"
#endif //USE_THREADS

#if defined(USE_TBB)
#include "tbb/task_scheduler_init.h"
#include "ParticleFilterTBB.h"
#include "TrackingModelTBB.h"
using namespace tbb;
#endif //USE_TBB

#include "ParticleFilter.h"
#include "TrackingModel.h"
#include "system.h"


using namespace std;

//templated conversion from string
template<class T>
bool num(const string s, T &n)
{	istringstream ss(s);
    ss >> n;
    return !ss.fail();
}

//write a given pose to a stream
inline void WritePose(ostream &f, vector<float> &pose)
{	for(int i = 0; i < (int)pose.size(); i++)
    f << pose[i] << " ";
    f << endl;
}

bool ProcessCmdLine(int argc, char **argv, string &path, int &cameras, int &frames, int &particles, int &layers, int &threads, int &threadModel, bool &OutputBMP)
{
    string    usage("Usage : Track (Dataset Path) (# of cameras) (# of frames to process)\n");
    usage += string("              (# of particles) (# of annealing layers) \n");
    usage += string("              [thread model] [# of threads] [write .bmp output (nonzero = yes)]\n\n");
    usage += string("        Thread model : 0 = Auto-select from available models\n");
    usage += string("                       1 = Intel TBB                 ");
#ifdef USE_TBB
    usage += string("\n");
#else
    usage += string("(unavailable)\n");
#endif
    usage += string("                       2 = Posix / Windows threads   ");
#ifdef USE_THREADS
    usage += string("\n");
#else
    usage += string("(unavailable)\n");
#endif
    usage += string("                       3 = OpenMP                    ");
#ifdef USE_OPENMP
    usage += string("\n");
#else
    usage += string("(unavailable)\n");
#endif
    usage += string("                       4 = Serial\n");

    string errmsg("Error : invalid argument - ");
    if(argc < 6 || argc > 9) //check for valid number of arguments
    {	cout << "Error : Invalid number of arguments" << endl << usage << endl;
        return false;
    }
    path = string(argv[1]); //get dataset path and add backslash if needed
    if(path[path.size() - 1] != DIR_SEPARATOR[0])
        path.push_back(DIR_SEPARATOR[0]);
    if(!num(string(argv[2]), cameras))//parse each argument
    {	cout << errmsg << "number of cameras" << endl << usage << endl; 
        return false; 
    }
    if(!num(string(argv[3]), frames))													
    {	cout << errmsg << "number of frames" << endl << usage << endl; 
        return false; 
    }
    if(!num(string(argv[4]), particles))												
    {	cout << errmsg << "number of particles" << endl << usage << endl;
        return false;
    }
    if(!num(string(argv[5]), layers))													
    {	cout << errmsg << "number of annealing layers" << endl << usage << endl;
        return false;
    }
    threads = -1;
    threadModel = 0;
    if(argc < 7)//use default single thread mode if no threading arguments present
        return true;
    if(!num(string(argv[6]), threadModel))
    {	
        cout << errmsg << "Thread Model" << endl << usage << endl;
        return false;
    }
    if(argc > 7) 
    {
        if(!num(string(argv[7]), threads))
        {	cout << errmsg << "number of threads" << endl << usage << endl;
            return false;
        }
    }

    int n;
    OutputBMP = true; // do not output bmp results by default
    if(argc > 8)
    {	
        if(!num(string(argv[8]), n))
        {	
            cout << errmsg << "Output BMP flag" << endl << usage << endl;
            return false;
        }
        OutputBMP = (n != 0);
    }
    return true;
}

//Body tracking threaded with OpenMP
#if defined(USE_OPENMP)
int mainOMP(string path, int cameras, int frames, int particles, int layers, int threads, bool OutputBMP)
{
    cout << "Threading with OpenMP" << endl;
    if(threads < 1) //Set number of threads used by OpenMP
        omp_set_num_threads(omp_get_num_procs()); //use number of processors by default
    else
        omp_set_num_threads(threads);
    cout << "Number of Threads : " << omp_get_max_threads() << endl;

    TrackingModelOMP model;
    if(!model.Initialize(path, cameras, layers)) //Initialize model parameters
    {	
        cout << endl << "Error loading initialization data." << endl;
        return 0;
    }
    model.SetNumThreads(threads);
    model.GetObservation(0); //load data for first frame
    //particle filter (OMP threaded) instantiated with body tracking model type
    ParticleFilterOMP<TrackingModel> pf; 
    pf.SetModel(model); //set the particle filter model
    //generate initial set of particles and evaluate the log-likelihoods
    pf.InitializeParticles(particles); 

    cout << "Using dataset : " << path << endl;
    cout << particles << " particles with " << layers << " annealing layers" << endl << endl;
    ofstream outputFileAvg((path + "poses.txt").c_str());

    vector<float> estimate; //expected pose from particle distribution

    // this is where timing should start
    for(int i = 0; i < frames; i++)//process each set of frames
    {	
        cout << "Processing frame " << i << endl;
        if(!pf.Update((float)i)) //Run particle filter step
        {	cout << "Error loading observation data" << endl;
            return 0;
        }		
        pf.Estimate(estimate); //get average pose of the particle distribution
        WritePose(outputFileAvg, estimate);
        if(OutputBMP)
            pf.Model().OutputBMP(estimate, i);//save output bitmap file
    }
    // this is where timing should end 

    return 1;
}
#endif

#if defined(USE_THREADS)
//Body tracking threaded with explicit Posix threads
int mainPthreads(string path, int cameras, int frames, 
                 int particles, int layers, int threads, bool OutputBMP)
{
    cout << "Threading with Posix Threads" << endl;
    if(threads < 1) {
        cout << "Warning: Illegal or unspecified number of threads, using 1 thread" << endl;
        threads = 1;
    }

    cout << "Number of threads : " << threads << endl;
    WorkPoolPthread workers(threads);//create thread work pool

    TrackingModelPthread model(workers); //register tracking model commands
    workers.RegisterCmd(workers.THREADS_CMD_FILTERROW, model);
    workers.RegisterCmd(workers.THREADS_CMD_FILTERCOLUMN, model);
    workers.RegisterCmd(workers.THREADS_CMD_GRADIENT, model);

    if(!model.Initialize(path, cameras, layers)) //Initialize model parameters
    {	cout << endl << "Error loading initialization data." << endl;
        return 0;
    }
    model.SetNumThreads(threads);
    model.SetNumFrames(frames);
    model.GetObservation(-1); //load data for first frame
    //particle filter instantiated with body tracking model type
    ParticleFilterPthread<TrackingModel> pf(workers); 
    pf.SetModel(model);	//set the particle filter model

    //register particle filter commands
    workers.RegisterCmd(workers.THREADS_CMD_PARTICLEWEIGHTS, pf); 
    workers.RegisterCmd(workers.THREADS_CMD_NEWPARTICLES, pf);//register 
    //generate initial set of particles and evaluate the log-likelihoods
    pf.InitializeParticles(particles);

    cout << "Using dataset : " << path << endl;
    cout << particles << " particles with " << layers << " annealing layers" << endl << endl;
    ofstream outputFileAvg((path + "poses.txt").c_str());

    vector<float> estimate; //expected pose from particle distribution

    // this is where timing should start
    for(int i = 0; i < frames; i++) //process each set of frames
    { 
     	cout << "Processing frame " << i << endl;
        if(!pf.Update((float)i)) //Run particle filter step
        {	cout << "Error loading observation data" << endl;
            workers.JoinAll();
            return 0;
        }		
        pf.Estimate(estimate); //get average pose of the particle distribution
        WritePose(outputFileAvg, estimate);
        if(OutputBMP)
            pf.Model().OutputBMP(estimate, i); //save output bitmap file
    }
    model.close();
    workers.JoinAll();
    // this is where timing should end 

    return 1;
}
#endif


#if defined(USE_TBB)
//Body tracking threaded with Intel TBB
int mainTBB(string path, int cameras, int frames, int particles, 
            int layers, int threads, bool OutputBMP)
{
    tbb::task_scheduler_init init(task_scheduler_init::deferred);
    cout << "Threading with TBB" << endl;

    if(threads < 1)
    {	init.initialize(task_scheduler_init::automatic);
        cout << "Number of Threads configured by task scheduler" << endl;
    }
    else
    {	
        init.initialize(threads); 
        cout << "Number of Threads : " << threads << endl;
    }

    TrackingModelTBB model;
    if(!model.Initialize(path, cameras, layers)) //Initialize model parameters
    {	
        cout << endl << "Error loading initialization data." << endl;
        return 0;
    }

    model.SetNumThreads(particles);
    model.SetNumFrames(frames);
    model.GetObservation(0); //load data for first frame

    //particle filter (TBB threaded) instantiated with body tracking model type
    ParticleFilterTBB<TrackingModelTBB> pf;

    pf.SetModel(model);	//set the particle filter model
    //generate initial set of particles and evaluate the log-likelihoods
    pf.InitializeParticles(particles);
    pf.setOutputBMP(OutputBMP);

    cout << "Using dataset : " << path << endl;
    cout << particles << " particles with " << layers << " annealing layers" << endl << endl;
    pf.setOutputFile((path + "poses.txt").c_str());
    ofstream outputFileAvg((path + "poses.txt").c_str());

    // Create the TBB pipeline - 1 stage for image processing, one for particle filter update
    tbb::pipeline pipeline;
    pipeline.add_filter(model);
    pipeline.add_filter(pf);
    pipeline.run(1);
    pipeline.clear();


    return 1;
}
#endif 


//Body tracking Single Threaded
int mainSingleThread(string path, int cameras, int frames, int particles, 
                     int layers, bool OutputBMP)
{
    cout << endl << "Running Single Threaded" << endl << endl;

    TrackingModel model;
    if(!model.Initialize(path, cameras, layers)) //Initialize model parameters
    {	cout << endl << "Error loading initialization data." << endl;
        return 0;
    }
    model.GetObservation(0); //load data for first frame
    //particle filter instantiated with body tracking model type
    ParticleFilter<TrackingModel> pf;
    pf.SetModel(model); //set the particle filter model
    //generate initial set of particles and evaluate the log-likelihoods
    pf.InitializeParticles(particles); 

    cout << "Using dataset : " << path << endl;
    cout << particles << " particles with " << layers << " annealing layers" << endl << endl;
    ofstream outputFileAvg((path + "poses.txt").c_str());

    vector<float> estimate; //expected pose from particle distribution

    // this is where timing should start
    for(int i = 0; i < frames; i++) //process each set of frames
    {	
        cout << "Processing frame " << i << endl;
        if(!pf.Update((float)i)) //Run particle filter step
        {	
            cout << "Error loading observation data" << endl;
            return 0;
        }		
        pf.Estimate(estimate); //get average pose of the particle distribution
        WritePose(outputFileAvg, estimate);
        if(OutputBMP)
            pf.Model().OutputBMP(estimate, i); //save output bitmap file
    }
    // this is where timing should end

    return 1;
}

int main(int argc, char **argv)
{
    string path;
    bool OutputBMP;
    //process command line parameters to get path, cameras, and frames
    int cameras, frames, particles, layers, threads, threadModel;

    if(!ProcessCmdLine(argc, argv, path, cameras, frames, particles, 
                       layers, threads, threadModel, OutputBMP))
        return 0;

    if(threadModel == 0) {
#if defined(USE_TBB)
        threadModel = 1;
#elif defined(USE_THREADS)
        threadModel = 2;
#elif defined(USE_OPENMP)
        threadModel = 3;
#else
        threadModel = 4;
#endif
    }

    switch(threadModel)
    {
        case 0 :
            //This case should never happen, 
            //we auto-select the thread model before this switch
            cout << "Internal error. Aborting." << endl;
            exit(1);
            break;

        case 1 :
#if defined(USE_TBB)
            //Intel TBB threads tracking
            mainTBB(path, cameras, frames, particles, layers, threads, OutputBMP);
            break;
#else
            cout << "Not compiled with Intel TBB support. " << endl;
            cout << "If the environment supports it, rebuild with USE_TBB #defined." << endl;
            break;  
#endif

        case 2 :
#if defined(USE_THREADS)
            //Posix threads tracking
            mainPthreads(path, cameras, frames, particles, layers, threads, OutputBMP);
            break;
#else
            cout << "Not compiled with Posix threads support. " << endl;
            cout << "If the environment supports it, rebuild with USE_THREADS #defined." << endl;
            break;
#endif

        case 3 : 
#if defined(USE_OPENMP)
            //OpenMP threaded tracking
            mainOMP(path, cameras, frames, particles, layers, threads, OutputBMP);
            break;
#else
            cout << "Not compiled with OpenMP support. " << endl;
            cout << "If the environment supports OpenMP, rebuild with USE_OPENMP #defined." << endl;
            break;
#endif

        case 4 :
            //single threaded tracking
            mainSingleThread(path, cameras, frames, particles, layers, OutputBMP);
            break;

        default : 
            cout << "Invalid thread model argument. " << endl;
            cout << "Thread model : 0 = Auto-select thread model" << endl;
            cout << "               1 = Intel TBB" << endl;
            cout << "               2 = Posix / Windows Threads" << endl;
            cout << "               3 = OpenMP" << endl;
            cout << "               4 = Serial" << endl;
            break;
    }

    return 0;
}
