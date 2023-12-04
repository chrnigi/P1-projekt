typedef enum GraphTypes {
    SCATTERPLOT,
    MONTHLYAVERAGE,
    COMPARISON,
    MAX_GRAPH_TYPE,
    
} GraphTypes;
// Changes made to this enum must also be reflected in the graph_type_strings array in drawGraph.c

typedef enum DataType {
    LOWPERCENT,
    RENEWPERCENT,
    CIDIRECT,
    CILCA,
    MAX_DATA_TYPE,
} DataType;
// Changes made to this enum must also be reflected in the type_strings char and wchar_t arrays in drawGraph.c


typedef struct GraphParams
{
    GraphTypes graph_type;
    DataType data_type;
    time_t day;
} GraphParams;

GraphParams graph_input();
void graph_exec(GraphParams input);
int graph_scatterplot_exec(DataType type, Datapoint *data, time_t day);
int graph_mon_avg_exec(DataType type, Datapoint *data);
int graph_multiple_exec(DataType type, Datapoint *data, time_t day);
