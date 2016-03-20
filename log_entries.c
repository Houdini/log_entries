#include "postgres.h"

#include "catalog/pg_type.h"
#include "commands/trigger.h"
#include "executor/spi.h"
#include "miscadmin.h"
#include "utils/builtins.h"
#include "utils/nabstime.h"
#include "utils/rel.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(log_entries);
#define MaxAttributes 2

Datum
log_entries(PG_FUNCTION_ARGS)
{
    Relation rel;
    TriggerData *trigger_data;
    Trigger *trigger;
    TupleDesc tupdesc;
    Datum current_user;
    HeapTuple rettuple;
    Datum newvals[MaxAttributes];
    int chattr[MaxAttributes];
 
    if (!CALLED_AS_TRIGGER(fcinfo))
        elog(ERROR, "log_entries should be called only as trigger");
    
    trigger_data = (TriggerData *) fcinfo->context;
    
    if (!TRIGGER_FIRED_FOR_ROW(trigger_data->tg_event))
        elog(ERROR, "log_entries should be fire only for row");
    
    if (TRIGGER_FIRED_BY_DELETE(trigger_data->tg_event))
        return (Datum) trigger_data->tg_trigtuple;
    
    trigger = trigger_data->tg_trigger;

    if (trigger->tgnargs != MaxAttributes)
        elog(ERROR, "log_entries, need two arguments");

    rel = trigger_data->tg_relation;
    tupdesc = rel->rd_att;
    
    if (SPI_gettypeid(tupdesc, SPI_fnumber(tupdesc, trigger->tgargs[0])) != ABSTIMEOID)
        elog(ERROR, "log_entries, first argument should be ABSTIME");

    if (SPI_gettypeid(tupdesc, SPI_fnumber(tupdesc, trigger->tgargs[1])) != TEXTOID)
        elog(ERROR, "log_entries, first argument should be TEXT");

    current_user = CStringGetTextDatum(GetUserNameFromId(GetUserId(), false));

    newvals[0] = GetCurrentAbsoluteTime();
    newvals[1] = current_user;
    chattr[0] = SPI_fnumber(tupdesc, trigger->tgargs[0]);
    chattr[1] = SPI_fnumber(tupdesc, trigger->tgargs[1]);

    rettuple = SPI_modifytuple(rel, trigger_data->tg_trigtuple, MaxAttributes, chattr, newvals, NULL);
    return PointerGetDatum(rettuple);
}