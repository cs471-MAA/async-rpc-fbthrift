from datetime import time
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


DATA_DIR = "container_files/"
CLIENT_FP = DATA_DIR + "client_stats.csv"
MSGSERV_FP = DATA_DIR + "message_service_stats.csv"
SANITSERV_FP = DATA_DIR + "sanitization_service_stats.csv"
MOCKDATA_FP = DATA_DIR + "mock_database_stats.csv"
DF_NAMES = ["query_uid", "epoch_time"]
DF_TYPES = {DF_NAMES[0]:"Uint64", DF_NAMES[1]:"Uint64"}
CLIENT_UID_SHIFT = 28
QUERY_INDEX_MASK = 0xFFFFFFF

################### STILL IN CONSTRUCTION, used for tests and design ###################

def treat_data(filepath, time_start=None, time_factor=1000.0):
    df = pd.read_csv(filepath, names=DF_NAMES, dtype=DF_TYPES)
    return_time_start = False
    if time_start is None:
        return_time_start = True
        time_start = df["epoch_time"].min()
    
    df["epoch_time"] -= time_start
    df["epoch_time"] /= time_factor
    df["client_uid"] = np.right_shift(df["query_uid"], CLIENT_UID_SHIFT)
    df["query_index"] = df["query_uid"] & QUERY_INDEX_MASK
    df.sort_values(["query_uid", "epoch_time"])
    # df = df.groupby(["query_uid", "client_uid", "query_index"]).agg(['min', 'max'])
    df = df.groupby(["query_uid", "client_uid", "query_index"]).agg([lambda x: x.to_numpy()[0],
                                                                     lambda x: np.nan if x.shape[0] < 2 else x.to_numpy()[1]])
    df.columns = df.columns.set_levels(['start','end'], level=1)
    df.columns = df.columns.get_level_values(1)
    df = df.reset_index()

    # print([df["start"] > df["end"]])
    if return_time_start:
        return df, time_start   
    else:
        return df

client_df, time_start = treat_data(CLIENT_FP)
msgserv_df = treat_data(MSGSERV_FP, time_start)
sanitserv_df = treat_data(SANITSERV_FP, time_start)
mockdata_df = treat_data(MOCKDATA_FP, time_start)


all = client_df.merge(msgserv_df, how='outer', on=["query_uid", "client_uid", "query_index"], suffixes=("_client", "_msg"))
all = all.merge(sanitserv_df, how='outer', on=["query_uid", "client_uid", "query_index"], suffixes=("", "_sanit"))
all = all.merge(mockdata_df, how='outer', on=["query_uid", "client_uid", "query_index"], suffixes=("_sanit", "_mock"))

all = all[["client_uid", "query_index", 
          "start_client", "start_msg", "start_sanit", "start_mock",
          "end_mock", "end_sanit", "end_msg", "end_client"]]

all = all.groupby("client_uid")

def plot_trace(X1, X2, Y, color, label, w):
    U = X2 - X1
    V = Y - Y
    plt.quiver(X1, Y, U, V,
               color=color, label=label, scale_units="xy",
               units="height", width=w, headwidth=0, scale=1)

for client_uid, queries in all:
    df = queries.drop("client_uid", axis=1)

    if df.end_client.isnull().values.any():
        continue
    # print(df)
    # plt.figure()
    # plt.plot(df.start_client, df.query_index, ".", color="#145ac9",label="Client Start")
    # plt.plot(df.end_client, df.query_index, ".", color="#2dbaed",label="Client End")

    w = 1/df.start_client.size #df.end_client.max() / 100
    print(f'{w=}')
    print(f'{df.end_client.max()=}')

    plot_trace(df.start_client, df.end_client, df.query_index, color="#145ac9", label="Client", w=w*0.99)

    plot_trace(df.start_msg, df.end_msg, df.query_index, color="#139415", label="Message Service", w=w*0.75)

    plot_trace(df.start_sanit, df.end_sanit, df.query_index, color="#bdb21c", label="Sanitize Service", w=w*0.5)

    plot_trace(df.start_mock, df.end_mock, df.query_index, color="#a12810", label="Mock database", w=w*0.25)

    plt.ylabel("query ID")
    plt.xlabel("time [ms]")
    plt.xlim(-df.end_client.max()*0.01, df.end_client.max()*1.01)
    plt.legend()
    plt.show()