#include <mutex>
#include <sstream>

#include <Profiler.h>


namespace {
std::mutex mutexMap;
}


namespace gv {
namespace support {


std::map<std::string, std::pair<int, int>> Profiler::map_;


Profiler::Profiler( const std::string& name, bool print, std::ostream& out )
    : name_( name )
    , print_( print )
    , out_( out )
    , tick_( std::chrono::steady_clock::now() )
{
    if ( print_ )
        out_ << "\nBegin profile. " << name_ << std::endl;
}


Profiler::~Profiler()
{
    const auto milli = currentMilli();
    if ( print_ )
        out_ << "End profile [" << milli << " ms]. " << name_ << "\n" << std::endl;
    addStatistic( name_, milli );
}


void Profiler::printStatistics( std::ostream& os )
{
    getStatistics( os );
}


void Profiler::getStatistics( std::ostream& os )
{
    std::lock_guard<std::mutex> lock( mutexMap );

    if ( map_.empty() )
    {
        os
            << "============================\n"
            << "== Profiler map is EMPTY! ==\n"
            << std::endl;
        return;
    }

    os
        << "==================\n"
        << "== Profiler map ==\n"
        << "\n";

    for ( const auto data : map_ )
    {
        int ms = std::get<0>( data.second );
        int count = std::get<1>( data.second );
        os
            << data.first << "\n"
            << "total ms: " << ms << "\n"
            << "count: " << count << "\n"
            << "avg: " << static_cast< double >( ms ) / count << "\n"
            << std::endl;
    }
}


std::string Profiler::getStatistics()
{
    std::ostringstream oss;
    getStatistics( oss );
    return oss.str();
}


void Profiler::addStatistic( const std::string& name, int milli )
{
    std::lock_guard<std::mutex> lock( mutexMap );

    auto it = map_.find( name );
    if ( it == map_.end() )
    {
        bool ok;
        std::tie( it, ok ) = map_.emplace( name, std::make_pair( 0, 0 ) );
        if ( !ok )
            return;
    }

    auto& par = it->second;
    std::get<0>( par ) += milli;
    ++std::get<1>( par );
}


std::size_t Profiler::currentMilli() const
{
    const auto tick = std::chrono::steady_clock::now();
    return static_cast< std::size_t >( std::chrono::duration_cast< std::chrono::milliseconds >( tick - tick_ ).count() );
}


}
}