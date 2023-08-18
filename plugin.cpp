#include <simPlusPlus/Plugin.h>
#include <simPlusPlus/Handles.h>
#include "plugin.h"
#include "stubs.h"
#include "config.h"
#include "UI.h"
#include "SIM.h"
#include "Bridge.h"
#ifdef Qt_Quick3D_FOUND
#include "Geometry.h"
#endif // Qt_Quick3D_FOUND

#include <QBuffer>
#include <QLibraryInfo>

class Plugin : public sim::Plugin
{
public:
    void onInit()
    {
        if(sim::getBoolParam(sim_boolparam_headless))
            throw std::runtime_error("doesn't work in headless mode");

        if(!registerScriptStuff())
            throw std::runtime_error("failed to register script stuff");

        setExtVersion("QML User-Interface Plugin");
        setBuildDate(BUILD_DATE);

        Bridge::registerQmlType();
#ifdef Qt_Quick3D_FOUND
        Geometry::registerQmlType();
#endif // Qt_Quick3D_FOUND

        sim = SIM::getInstance();
    }

    void onCleanup()
    {
        SIM::destroyInstance();
        sim = nullptr;
    }

    void onUIInit()
    {
        auto w = nullptr;//reinterpret_cast<QWidget*>(sim::getMainWindow(1));
        ui = UI::getInstance(w);
    }

    void onUICleanup()
    {
        UI::destroyInstance();
        ui = nullptr;
    }

    void onInstanceSwitch(int sceneID)
    {
        if(sceneID == oldSceneID) return;

        for(auto engine : handles.findByScene(oldSceneID))
            sim->sendEventToQML(engine, "onInstanceSwitch", "false");

        for(auto engine : handles.findByScene(sceneID))
            sim->sendEventToQML(engine, "onInstanceSwitch", "true");

        oldSceneID = sceneID;
    }

    void onScriptStateDestroyed(int scriptID)
    {
        for(auto engine : handles.find(scriptID))
            sim->destroyEngine(handles.remove(engine));
    }

    void createEngine(createEngine_in *in, createEngine_out *out)
    {
        QStringList importPaths;
        importPaths << QString::fromStdString(sim::getStringParam(sim_stringparam_resourcesdir) + "/qml");
        importPaths << QString::fromStdString(sim::getStringParam(sim_stringparam_scene_path));
        QQmlApplicationEngine *engine;
        sim->createEngine(&engine, importPaths);
        out->handle = handles.add(engine, in->_.scriptID);
        sim->setEngineHandle(engine, QString::fromStdString(out->handle));
    }

    void destroyEngine(destroyEngine_in *in, destroyEngine_out *out)
    {
        auto engine = handles.get(in->handle);
        sim->destroyEngine(handles.remove(engine));
    }

    QString getContextInfo(const SScriptCallBack &_)
    {
        return QString("%1:%2").arg(_.source).arg(_.line);
    }

    void load(load_in *in, load_out *out)
    {
        auto engine = handles.get(in->engineHandle);
        auto filename = QString::fromStdString(in->filename);
        sim->load(engine, filename, getContextInfo(in->_));
    }

    void loadData(loadData_in *in, loadData_out *out)
    {
        auto engine = handles.get(in->engineHandle);
        QByteArray data(in->data.c_str(), in->data.length());
        QString basepath(QString::fromStdString(in->basepath));
        sim->loadData(engine, data, basepath, getContextInfo(in->_));
    }

    void setEventHandlerRaw(setEventHandlerRaw_in *in, setEventHandlerRaw_out *out)
    {
        auto engine = handles.get(in->engineHandle);
        auto functionName = QString::fromStdString(in->functionName);
        sim->setEventHandler(engine, in->_.scriptID, functionName);
    }

    void sendEventRaw(sendEventRaw_in *in, sendEventRaw_out *out)
    {
        auto engine = handles.get(in->engineHandle);
        auto eventName = QString::fromStdString(in->eventName);
        QByteArray data(in->eventData.c_str(), in->eventData.length());
        sim->sendEventToQML(engine, eventName, data);
    }

    void imageDataURL(imageDataURL_in *in, imageDataURL_out *out)
    {
        auto imageFormat = imageFormats.find(in->format);
        if(imageFormat == imageFormats.end())
            throw sim::exception("invalid format: \"%s\"", in->format);
        auto imageDataFormat = imageDataFormats.find(in->data_format);
        if(imageDataFormat == imageDataFormats.end())
            throw sim::exception("invalid data format: \"%d\"", in->data_format);
        QByteArray data(in->data.c_str(), in->data.length());
        QImage im((const uchar *)data.data(), in->width, in->height, imageDataFormat->second);
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        im.save(&buffer, imageFormat->first.c_str());
        QByteArray data_b64 = ba.toBase64();
        out->dataURL = "data:";
        out->dataURL += imageFormat->second;
        out->dataURL += ";base64,";
        out->dataURL += std::string(data_b64.data());
    }

    void qtVersion(qtVersion_in *in, qtVersion_out *out)
    {
        auto v = QLibraryInfo::version().segments();
        out->version = std::vector<int>(v.constBegin(), v.constEnd());
    }

private:
    int oldSceneID = -1;
    UI *ui;
    SIM *sim;
    sim::Handles<QQmlApplicationEngine*> handles{"QML.QQmlApplicationEngine"};
    const std::map<std::string, std::string> imageFormats{
        {"PNG", "image/png"},
        {"BMP", "image/bmp"},
        {"JPG", "image/jpeg"}
    };
    const std::map<int, QImage::Format> imageDataFormats{
        {simqml_image_data_format_gray8,    QImage::Format_Grayscale8},
        {simqml_image_data_format_rgb888,   QImage::Format_RGB888},
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        {simqml_image_data_format_bgr888,   QImage::Format_BGR888},
#endif
        {simqml_image_data_format_rgbx8888, QImage::Format_RGBX8888},
        {simqml_image_data_format_rgb32,    QImage::Format_RGB32},
        {simqml_image_data_format_argb32,   QImage::Format_ARGB32}
    };
};

SIM_UI_PLUGIN(Plugin)
#include "stubsPlusPlus.cpp"
