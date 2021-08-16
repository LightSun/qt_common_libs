# qt常用的库
比如qt网络请求的封装。
```c++
void FgsbServerApi::uploadMarks(const QStringList& marks, std::function<void(bool,const QString&)>&& cb){
     emit _log(H7_LOG_LEVEL_DEBUG, "req uploadMarks()");
     FgsbServiceInfo si = LocalConfig::get()->getServiceInfo();

    FUNC_SHARED_PTR(void(bool,const QString&)) ptr = FUNC_MAKE_SHARED_PTR_2(void(bool,const QString&), cb);
    newNetworkCom().url(si.domain + "/api/sys/coordinate/addJsonData")
            .header(QString("content-type"), QString("application/json"))
            .header("Authorization", sToken)
            .body([=]NETWORK_FUNC_BODY({
                          Q_UNUSED(url)
                          Q_UNUSED(form)
                          QJsonArray array;
                          for(int i = 0 ; i < marks.size() ; i ++){
                              array.append(marks[i]);
                          }
                          QByteArray json = newJsonBuilder().pair("stringList", array)
                              .toBytes();
                          qDebug() << "req json: " << json;
                          bytes.append(json);
                 })
            )
            .callback_error([=]NETWORK_FUNC_CALLBACK_ERROR(
                        {qDebug() << msg;
                                (*ptr)(false, msg);
                                }
                                )

    )
            .callback([ptr](NetworkComponent&,QString&, QByteArray& buffer)
                        {
                            RES_JSON_OBJECT_FUNC(ptr);
                            if(jobj["code"].toInt() == 0){
                                (*ptr)(true, QString(""));
                            }else{
                                (*ptr)(false, jobj["message"].toString());
                            }
                        }
                                )
            .post();
```
