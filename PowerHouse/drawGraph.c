#include "stdincludes.h"
#include "pbPlots/pbPlots.h"
#include "pbPlots/supportLib.h"
#include "csvRead.h"
#include "drawGraph.h"

int graph_scatterplot_exec(DataType type, Datapoint *data, time_t day)
{
    double hours[24];
    double datapoints[24];
    bool successfull_print;

    ScatterPlotSeries *series = GetDefaultScatterPlotSeriesSettings();
    series->xs = hours;
    series->xsLength = 24;
    series->ys = datapoints;
    series->ysLength = 24;

    ScatterPlotSettings *settings = GetDefaultScatterPlotSettings();
    settings->width = 1280;
    settings->height = 720;
    settings->autoBoundaries = true;
    settings->autoPadding = true;
    ScatterPlotSeries *s[] = {series};
    settings->xLabel = L"Hours";
    settings->xLabelLength = wcslen(settings->xLabel);
    settings->scatterPlotSeries = s;
    settings->scatterPlotSeriesLength = 1;

    wchar_t type_strings[MAX_DATA_TYPE][50] = {
        {L"Low Percent"},
        {L"Renewable Percent"},
        {L"Carbon Intensity Direct"},
        {L"Carbon Intensity LCA"},
    };
    
    size_t type_strings_len[MAX_DATA_TYPE];
    
    for (int i = 0; i < MAX_DATA_TYPE; i++)
    {
        type_strings_len[i] = wcslen(type_strings[i]);
    }
    wchar_t wcdate_string[128];
    char date_string[128];

    strftime(date_string, 128, "%Y/%m/%d ", localtime(&day));
    mbstowcs(wcdate_string, date_string, 128);

    wcscat(wcdate_string, type_strings[type]);

    settings->title = wcdate_string;
    settings->titleLength = wcslen(wcdate_string);

    // Fill hours[] with 0 to 23
    for (int i = 0; i < 24; i++)
    {
        hours[i] = i;
    }

    int i = 0;
    while (data[i].datetime != day) 
    {
        if(i == 8760) break;
        i++;
    }
    if(i == 8760) return 1;
    
    

    switch (type)
    {
    case LOWPERCENT:
    {
        for (int j = 0; j < 24; j++)
        {
            datapoints[j] = data[j+i].low_percent;
        }
        settings->yLabel = L"%";
        settings->yLabelLength = wcslen(settings->yLabel);
        
    } break;

    case RENEWPERCENT:
    {
        for (int j = 0; j < 24; j++)
        {
            datapoints[j] = data[j+i].renew_percent;
        }   
        settings->yLabel = L"%";
        settings->yLabelLength = wcslen(settings->yLabel);

    } break;

    case CIDIRECT:
    {
        for (int j = 0; j < 24; j++)
        {
            datapoints[j] = data[j+i].ci_direct;
        }
        settings->yLabel = L"gCO2eq";
        settings->yLabelLength = wcslen(settings->yLabel);

    } break;

    case CILCA:
    {
        for (int j = 0; j < 24; j++)
        {
            datapoints[j] = data[j+i].ci_lca;
        }

        settings->yLabel = L"gCO2eq";
        settings->yLabelLength = wcslen(settings->yLabel);

    } break;

    default:
        return 1;
    }

    wchar_t msg[] = L"Error printing graph!\n";
    size_t msglen = wcslen(msg);

    RGBABitmapImageReference *canvasref = CreateRGBABitmapImageReference();
    StringReference *errmsg = CreateStringReference(msg, msglen);
    successfull_print = DrawScatterPlotFromSettings(canvasref, settings, errmsg);


    if (successfull_print)
    {
        size_t length;
        double *pngdata = ConvertToPNG(&length, canvasref->image);
        WriteToFile(pngdata, length, "graph.png");
        // Deallocates memory
        DeleteImage(canvasref->image); 
    }
    else
    {
        fprintf(stderr, "Error: ");
        for (int i = 0; i < errmsg->stringLength; i++)
        {
            fprintf(stderr, "%c", errmsg->string[i]);
        }
        fprintf(stderr, "\n");
    }
}

GraphParams graph_input()
{

    char GraphType_strings[MAX_GRAPH_TYPE][50] = {
        {"Scatterplot"},
        {"Monthly Average"},
        {"Comparison"},
    };

    char DataType_strings[MAX_DATA_TYPE][50] = {
        {"Low Percent"},
        {"Renewable Percent"},
        {"Carbon Intensity Direct"},
        {"Carbon Intensity LCA"},
    };

    tm day;
    day.tm_hour = 0;
    day.tm_min = 0;
    day.tm_sec = 0;
    day.tm_isdst = -1;

    GraphParams input;
    
    printf("Choose a which type of graph you wish to be printed:\n\n");
    
    for (int i = 0; i < MAX_GRAPH_TYPE; i++)
    {
        printf("%d. %s\n", i+1, GraphType_strings[i]);
    }

    scanf(" %d", &input.graph_type);
    input.graph_type--;
    while (input.graph_type >= MAX_GRAPH_TYPE) 
    {
        printf("Please choose a valid graph type.\n");
        scanf(" %d", &input.graph_type);
        input.graph_type--;
    }

    printf("You have chosen: ");
    printf("%s\n", GraphType_strings[input.graph_type]);
    
    printf("Which data do you want to graph?\n\n");

    for (int i = 0; i < MAX_DATA_TYPE; i++)
    {
        printf("%d. %s\n", i+1, DataType_strings[i]);
    }
    
    scanf(" %d", &input.data_type);
    input.data_type--;
    while (input.data_type >= MAX_DATA_TYPE)
    {
        printf("Please choose a valid data type.\n");
        scanf(" %d", &input.data_type);
        input.data_type--;
    }
    
    printf("You have chosen: ");
    printf("%s\n", DataType_strings[input.data_type]); 

    if (input.graph_type <= SCATTERPLOT)
    {
        printf("Which day do you wish to see the graph for?\n");
        printf("Please input in format yyyy-MM-dd\n");
        scanf(" %d-%d-%d", &day.tm_year, &day.tm_mon, &day.tm_mday);
    
        day.tm_year -= 1900;
        day.tm_mon -= 1;
        input.day = mktime(&day);    
    }
    else 
    {
        input.day = NULL;
    }
    
    return input;
}

void graph_exec(GraphParams input)
{
    int total_rows;
    Datapoint* data = readCSV("datafiler/DK-DK2_2022_hourly.csv", &total_rows, true);

    switch (input.graph_type)
    {
    case SCATTERPLOT:
        graph_scatterplot_exec(input.data_type, data, input.day);
        break;
    
    case MONTHLYAVERAGE:
        graph_mon_avg_exec(input.data_type, data);
        break;
    default:
        break;
    }

    system("graph.png");
    free(data);
}

int graph_mon_avg_exec(DataType type, Datapoint *data)
{
    double months[12];
    double averages[12];

    for (int i = 0; i < 12; i++) months[i] = i + 1;

    int j = 0;
    for (size_t i = 0; i < 12; i++)
    {
        double sum = 0;
        int k = 0;
        while (localtime(&data[j].datetime)->tm_mon == i)
        {
            switch (type)
            {
            case LOWPERCENT:
                sum += data[j].low_percent;
                break;
            case RENEWPERCENT:
                sum += data[j].renew_percent;
                break;
            case CIDIRECT:
                sum += data[j].ci_direct;
                break;
            case CILCA:
                sum += data[j].ci_lca;
                break;
            default:
                break;
            }
            k++;
            j++;
        }
        averages[i] = sum/k;
        
    }
    
    wchar_t err[] = L"Error printing monthly average!";

    RGBABitmapImageReference *canvasref = CreateRGBABitmapImageReference();
    StringReference *errmsg = CreateStringReference(err, wcslen(err));
    bool success = DrawScatterPlot(canvasref, 1280, 720, months, 12, averages, 12, errmsg);

    if (success) {
        size_t len;
        double *pngdata = ConvertToPNG(&len, canvasref->image);
        WriteToFile(pngdata, len, "graph.png");
        DeleteImage(canvasref->image);
    } 
    else {
        fprintf(stderr, "Error:");
        for (int i = 0; i < errmsg->stringLength; i++)
        {
            fprintf(stderr, "%c", errmsg->string[i]);
        }
        fprintf(stderr, "\n");
        return 1;
        
    }
    return 0;
}
