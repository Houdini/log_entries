-- ===================================================================
-- create log_entries
-- ===================================================================
create extension log_entries;
create schema "log_entries_schema";
create table "log_entries_schema".test_table (
  id int not null,
  data text,
  updated_at abstime,
  updated_by text,
  constraint id_pkey primary key (id)
);

insert into "log_entries_schema".test_table values(1, '', null, 'text');
select id, updated_at, updated_by from "log_entries_schema".test_table;

-- log_entries should be called as trigger
select log_entries();

-- test trigger
create trigger log_entries
  before insert or delete or update on "log_entries_schema".test_table
  for each row execute procedure log_entries(updated_at, updated_by);

insert into "log_entries_schema".test_table values(2, null, null);
select id, updated_by from "log_entries_schema".test_table where id = 2;
select count(*) from "log_entries_schema".test_table where updated_at is not null and id = 2;

update "log_entries_schema".test_table set data = 'data' where id = 1;
select count(*) from "log_entries_schema".test_table where updated_at is null;
select count(*) from "log_entries_schema".test_table where id = 1 and updated_at is null;
delete from "log_entries_schema".test_table;
select * from "log_entries_schema".test_table;
